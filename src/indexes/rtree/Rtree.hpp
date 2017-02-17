#pragma once
#include "bench/LazyDataSet.hpp"
#include "bench/SpatialIndex.hpp"
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
 */
template <class N>
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
		 * Load the given data set and construct the tree from it.
		 */
		void load(LazyDataSet& dataSet)
		{
			if (!dataSet.getSize()) {
				return;
			}

			for (auto& object : dataSet) {
				insert(object);
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


	protected:

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
			stats["height"] = height;

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
						N * node = entry.node;
						path.emplace(node->entries, node->nEntries);

						if (path.size() == height - 1) {
							stats["leaf_accesses"]++;
						}

						stats["node_accesses"]++;
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
