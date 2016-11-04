#pragma once
#include "bench/LazyDataSet.hpp"
#include "bench/SpatialIndex.hpp"
#include "Entry.hpp"
#include "KnnQueueEntry.hpp"
#include "Mbr.hpp"
#include "Node.hpp"
#include <algorithm>
#include <forward_list>
#include <queue>
#include <stack>


namespace Rtree
{

/**
 * R-tree algorithms without optimizations.
 *
 * Does not consider disk pages, as this is memory resident anyway.
 *
 * @tparam D Dimensionality
 * @tparam C Maximal number of entries in each node
 */
template <unsigned D, unsigned C>
class SpatialIndex : public ::SpatialIndex
{
	using M = Mbr<D>;
	using N = Node<D, C>;
	using E = Entry<D, C>;
	using Id = DataObject::Id;

	public:

		/**
		 * Construct a new index from the given data set.
		 */
		SpatialIndex(LazyDataSet& dataSet)
		{
			if (dataSet.empty()) {
				return;
			}

			root = allocate();
			height = 1;

			for (auto& object : dataSet) {
				insert(object);
			}
		};

		SpatialIndex()
		{
			for (N * node : nodes) {
				delete node;
			}
		};


	protected:

		/**
		 * Range search with Guttman's algorithm.
		 */
		Results rangeSearch(const AxisAlignedBox& box) const
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


		/**
		 * Insert a data object in the tree.
		 *
		 * @param object DataObject to insert
		 */
		void insert(const DataObject& object)
		{
			const Point& point = object.getPoint();
			std::vector<E *> path;

			N * node = root;

			// Find leaf node
			for (unsigned i = 0; i < height - 1; i++) {
				E& entry = node->leastEnlargement(point);

				// Update MBR
				entry.mbr += M(point);

				// Add to path and recurse
				path.push_back(&entry);
				node = entry.node;
			}

			// Add object
			if (node->isFull()) {
				split(path, object);
			} else {
				node->add(object);
			}
		};


		/**
		 * Split nodes and insert the new entry when done.
		 *
		 * The method starts at the bottom node and splits it if necessary. If
		 * the parent node is full, it is plit as well. Should there be no more
		 * nodes to be split, a new root node is created.
		 *
		 * @param path List of entries from top of tree to bottom
		 * @param newEntry New entry to insert at the bottom level
		 */
		void split(std::vector<E *> path, const E& newEntry)
		{
			E e = newEntry;
			auto top = path.rbegin();

			while (top != path.rend() && (*(top))->node->isFull()) {
				e = (*top)->split(e, allocate());
				top++;
			}

			// Can we add to an internal node (not root)?
			if (top != path.rend()) {
				(*top)->node->add(e);
				return;
			}

			// Can we add to the root?
			if (!root->isFull()) {
				root->add(e);
				return;
			}

			// All else fails - split root
			splitRoot(e);
		};


		/**
		 * Split the root node to make room for the new entry.
		 *
		 * @param newEntry New entry to add
		 */
		void splitRoot(const E& newEntry)
		{
			E oldRoot (root, M());
			root = allocate();
			root->add(oldRoot);

			// This will also recalculate the MBR for oldRoot
			root->add(
					root->entries[0].split(newEntry, allocate())
				);

			height += 1;
		}


		/**
		 * Allocate a new node.
		 * The node will be deleted with this object.
		 *
		 * @return New node
		 */
		N * allocate()
		{
			N * node = new N();
			nodes.push_front(node);
			return node;
		};
};

}
