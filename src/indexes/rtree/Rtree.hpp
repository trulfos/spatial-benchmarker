#pragma once
#include "bench/LazyDataSet.hpp"
#include "bench/SpatialIndex.hpp"
#include "bench/InvalidStructureError.hpp"
#include "KnnQueueEntry.hpp"
#include "Mbr.hpp"
#include <algorithm>
#include <forward_list>
#include <queue>
#include <stack>


namespace Rtree
{

/**
 * R-tree algorithms without optimizations.
 * Does not consider disk pages, as this is memory resident anyway.
 *
 * @tparam N Node type
 * @tparam m Minimum node children
 */
template <class N, unsigned m = 1>
class Rtree : public ::SpatialIndex
{
	public:
		using E = typename N::Entry;
		using M = typename E::M;
		using Id = DataObject::Id;

		/**
		 * Construct a new index from the given data set.
		 */
		Rtree()
		{
			root = allocateNode();
			height = 1;
		};

		virtual ~Rtree()
		{
			for (N * node : nodes) {
				delete node;
			}
		};


		/**
		 * Insert a data object into this index.
		 *
		 * @param object Data object to insert
		 */
		virtual void insert(const DataObject& object) = 0;


		/**
		 * Get the tree height.
		 *
		 * @return Height of tree
		 */
		const unsigned& getHeight() const
		{
			return height;
		};


		/**
		 * Get root node.
		 *
		 * @return Pointer to root
		 */
		N * getRoot()
		{
			return root;
		};


		/**
		 * Allocate a new node.
		 * The node will be deleted with this object.
		 *
		 * @return New node
		 */
		N * allocateNode()
		{
			N * node = new N();
			nodes.push_front(node);
			return node;
		};


		/**
		 * Add a new level by replacing the root.
		 */
		void addLevel(N * newRoot)
		{
			root = newRoot;
			height++;
		};


		/**
		 * Sanity checks on the structure of this R-tree.
		 *
		 * Currently checks the following:
		 *  - All MBRs are contained within their parents MBR,
		 *  - MBRs are as tight as possible and
		 *  - the upper and lower number of children is respected
		 */
		void checkStructure() const override
		{
			traverse([&](const E& entry, unsigned level) {
				const unsigned& nEntries = entry.node->nEntries;

				// These restrictions are not relevant for the root
				if (level == 0) {
					if (nEntries < 2) {
						throw InvalidStructureError(
								"The root node has less than 2 children"
							);
					}

					return true;
				}


				// Check child count
				if (nEntries > N::capacity) {
					throw InvalidStructureError(
							"Too many children of node"
						);
				}

				if (nEntries < m) {
					throw InvalidStructureError(
							"Too few children of node"
						);
				}


				// Check MBR containment
				M mbr = entry.begin()->mbr;

				for (const auto& e : entry) {
					if (!entry.mbr.contains(e.mbr)) {
						throw InvalidStructureError(
								"Node not contained within parent"
							);
					}

					mbr += e.mbr;
				}

				if (mbr != entry.mbr) {
					throw InvalidStructureError("MBR not tight");
				}


				return true;
			});
		}


		/**
		 * Collects tree statistics.
		 *
		 * Walks through the tree and e.g. counts the number of nodes at each
		 * level.
		 *
		 * @return Statistics collected
		 */
		StatsCollector collectStatistics() const override
		{
			StatsCollector stats;

			stats["height"] = height;
			stats["nodes"] = 0;
			stats["level_" + std::to_string(height)] = 1;

			traverse([&](const E& entry, unsigned level) {
				std::string key = "level_" + std::to_string(height - level - 1);
				stats[key] += entry.node->nEntries;
				stats["nodes"]++;
				return true;
			});

			return stats;
		};




	protected:

		/**
		 * Traverses the entire tree and executes the visitor for each entry.
		 *
		 * The tree is traversed in preorder and executes the visitor with the
		 * entry and level as arguments. The children of an entry is visited if
		 * the visitor returns true. Data entrys are not visited and a fake
		 * entry is created for the root.
		 *
		 * @param visitor Function taking a entry and the level (depth)
		 */
		template<class F>
		void traverse(F visitor) const
		{
			std::vector<std::pair<E *, unsigned>> path;
			path.emplace_back(root->entries, root->nEntries);

			E rootEntry (root, M());

			// Call for root
			if (!visitor(rootEntry, path.size() - 1)) {
				return;
			}

			while (!path.empty()) {
				auto& top = path.back();

				if (top.second == 0) {
					path.pop_back();
					continue;
				}

				top.second -= 1;
				E& entry = *(top.first++);

				// Call visitor and push children
				if (path.size() < height && visitor(entry, path.size() - 1)) {
					N * node = entry.node;
					path.emplace_back(node->entries, node->nEntries);
				}
			}
		}

		/**
		 * Range search with Guttman's algorithm.
		 */
		Results rangeSearch(const Box& box) const
		{
			Results resultSet;
			std::stack<std::pair<E *, unsigned>> path;

			path.emplace(root->entries, root->nEntries);

			while (!path.empty()) {
				auto& top = path.top();

				if (top.second == 0) {
					path.pop();
					continue;
				}

				top.second -= 1;
				E& entry = *(top.first++);

				if (entry.mbr.intersects(box)) {
					if (path.size() == height) {
						resultSet.push_back(entry.id);
					} else {
						N * node = entry.node;
						path.emplace(node->entries, node->nEntries);
					}
				}
			}

			return resultSet;
		};


		/**
		 * Range search with Guttman's algorithm - with instrumentation.
		 */
		Results rangeSearch(const Box& box, StatsCollector& stats) const
		{
			Results resultSet;
			std::stack<std::pair<E *, unsigned>> path;

			path.emplace(root->entries, root->nEntries);

			stats["iterations"] = 0;
			stats["leaf_accesses"] = (height == 1 ? 1 : 0);
			stats["node_accesses"] = 1;

			while (!path.empty()) {
				stats["iterations"]++;

				auto& top = path.top();

				if (top.second == 0) {
					path.pop();
					continue;
				}


				top.second -= 1;
				E& entry = *(top.first++);

				if (entry.mbr.intersects(box)) {
					if (path.size() == height) {
						resultSet.push_back(entry.id);
					} else {
						stats["node_accesses"]++;

						if (path.size() == height - 1) {
							stats["leaf_accesses"]++;
						}

						N * node = entry.node;
						path.emplace(node->entries, node->nEntries);
					}
				}

			}

			stats["results"] = resultSet.size();

			return resultSet;
		};


		/**
		 * Knn search using the optimal algorithm in the number of nodes
		 * accessed.
		 */
		Results knnSearch(unsigned k, const Point& point) const
		{
			Results results;
			M reference (point);
			std::priority_queue<
					KnnQueueEntry<N>,
					std::vector<KnnQueueEntry<N>>,
					std::greater<KnnQueueEntry<N>>
				> queue;

			queue.emplace(root, height, 0.0f);

			while (!queue.empty() && results.size() < k) {
				const auto& top = queue.top();

				unsigned elevation = top.elevation;
				N * node = top.node;
				Id id = top.id;

				queue.pop();

				if (elevation == 0) {
					results.push_back(id);
				} else if (elevation == 1) {
					for (auto& e : *node) {
						queue.emplace(e.id, 0, e.mbr.distance2(reference));
					}
				} else {
					unsigned el = elevation - 1;

					for (auto& e : *node) {
						queue.emplace(e.node, el, e.mbr.distance2(reference));
					}
				}
			}

			return results;
		};

	private:

		std::forward_list<N *> nodes;
		unsigned height;
		N * root;

};

}
