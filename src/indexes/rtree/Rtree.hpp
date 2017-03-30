#pragma once
#include "bench/LazyDataSet.hpp"
#include "common/SpatialIndex.hpp"
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
				const unsigned& size = entry.node->size();

				if (size > N::capacity) {
					throw InvalidStructureError(
							"Too many children of node"
						);
				}

				if (size < (level == 1 ? 2 : m)) {
					throw InvalidStructureError(
							"Too few children at level " + std::to_string(level)
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
				stats[key] += entry.node->size();
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
			using EIt = typename E::const_iterator;

			std::vector<std::pair<EIt, EIt>> path {{&root, &root + 1}};

			while (!path.empty()) {
				auto& top = path.back();

				if (top.first == top.second) {
					path.pop_back();
					continue;
				}

				const E& entry = *(top.first++);

				// Call visitor
				bool descend = visitor(entry, path.size());

				// Push children (if they exist)
				if (descend && path.size() < this->getHeight()) {
					path.emplace_back(entry.begin(), entry.end());
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
			stats["contained"] = 0;
			stats["includes"] = 0;
			stats["cut"] = 0;
			stats["unnecessary_comparisons"] = 0;

			stats["skipped"] = 0;
			unsigned lastLevel = 0;
			bool descended = false;

			traverse([&](const E& entry, unsigned level) {
					// Detect skipped nodes
					if (level > lastLevel) {
						descended = false;
					} else 	if (level < lastLevel && !descended) {
						descended = true;
						stats["skipped"]++;
					}

					descended |= (level == height);
					lastLevel = level;

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

					// Count skippable checks
					M qMbr (box);

					for (unsigned d = 0; d < E::dimension; ++d) {
						if (qMbr.getTop()[d] >= entry.mbr.getTop()[d]) {
							stats["unnecessary_comparisons"]++;
						}

						if (qMbr.getBottom()[d] <= entry.mbr.getBottom()[d]) {
							stats["unnecessary_comparisons"]++;
						}


					}

					if (qMbr.contains(entry.mbr)) {
						stats["contained"]++;
					}

					if (entry.mbr.contains(qMbr)) {
						stats["includes"]++;
					}

					if (qMbr.intersectionComplexity(entry.mbr) == 1) {
						stats["cut"]++;
					}

					// Count node accesses
					stats["node_accesses"]++;
					return true;
				});

			// Never reached the bottom after last descend?
			if (!descended) {
				stats["skipped"]++;
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
			throw std::runtime_error("k-NN search not implemented");
		};

	private:

		std::forward_list<N *> nodes;
		unsigned height;
		E root;

};

}
