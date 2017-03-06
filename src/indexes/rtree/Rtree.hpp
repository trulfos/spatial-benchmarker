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
		Rtree() : height(0)
		{
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
		E& getRoot()
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
		void addLevel(const E& newRoot)
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
				// Skip leafs
				if (level == height) {
					return false;
				}

				const unsigned& nEntries = entry.node->nEntries;

				// Check MBR containment
				M mbr = entry.begin()->mbr;

				for (const auto& e : entry) {
					if (!entry.mbr.contains(e.mbr)) {
						throw InvalidStructureError(
								"Node not contained within parent at level " +
								std::to_string(level)
							);
					}

					mbr += e.mbr;
				}

				if (mbr != entry.mbr) {
					throw InvalidStructureError("MBR not tight");
				}


				// Check child count
				if (nEntries > N::capacity) {
					throw InvalidStructureError(
							"Too many children of node"
						);
				}

				if (nEntries < (level == 1 ? 2 : m)) {
					throw InvalidStructureError(
							"The root node has less than 2 children"
						);
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
				// Skip leafs
				if (level == height) {
					return false;
				}

				std::string key = "level_" + std::to_string(height - level);
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
			assert(height > 0);
			std::vector<std::pair<const E *, unsigned>> path {{&root, 1}};

			while (!path.empty()) {
				auto& top = path.back();

				if (top.second == 0) {
					path.pop_back();
					continue;
				}

				top.second -= 1;
				const E& entry = *(top.first++);

				// Call visitor
				bool descend = visitor(entry, path.size());

				// Push children (if they exist)
				if (descend && path.size() < this->getHeight()) {
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
			assert(height > 0);
			Results resultSet;

			traverse([&](const E& entry, unsigned level) {
					// Skip nodes not overlapping
					if (!entry.mbr.intersects(box)) {
						return false;
					}

					// Push result if leaf
					if (level == height) {
						resultSet.push_back(entry.id);
						return false;
					}

					return true;
				});

			return resultSet;
		};


		/**
		 * Range search with Guttman's algorithm - with instrumentation.
		 */
		Results rangeSearch(const Box& box, StatsCollector& stats) const
		{
			Results resultSet;
			stats["leaf_accesses"] = 0;
			stats["node_accesses"] = 0;

			traverse([&](const E& entry, unsigned level) {
					// Skip nodes not overlapping
					if (!entry.mbr.intersects(box)) {
						return false;
					}

					// Count leaf accesses
					if (level == height - 1) {
						stats["leaf_accesses"]++;
					}

					// Push result if leaf
					if (level == height) {
						resultSet.push_back(entry.id);
						return false;
					}

					// Count node accesses
					stats["node_accesses"]++;
					return true;
				});

			stats["results"] = resultSet.size();
			return resultSet;
		};


		/**
		 * Knn search using the optimal algorithm in the number of nodes
		 * accessed.
		 */
		Results knnSearch(unsigned k, const Point& point) const
		{
			throw std::runtime_error("k-NN search not implemented");
		};

	private:

		std::forward_list<N *> nodes;
		unsigned height;
		E root;

};

}
