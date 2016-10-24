#pragma once
#include "../SpatialIndex.hpp"
#include "../../common/DataSet.hpp"
#include "Entry.hpp"
#include "Node.hpp"
#include "Mbr.hpp"
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <stack>
#include <queue>
#include <forward_list>


namespace Rtree
{

/**
 * R-tree algorithms without optimizations.
 *
 * Does not consider disk pages, as this is memory resident anyway.
 *
 * Template parameters:
 *  - D is the dimensionality
 *  - C is the maximal number of entries in each node
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
		SpatialIndex(const DataSet& dataSet)
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

		~SpatialIndex()
		{
			for (N * node : nodes) {
				delete node;
			}
		};


	protected:

		/**
		 * Range search with Guttman's algorithm.
		 */
		ResultSet rangeSearch(const AxisAlignedBox& box) const
		{
			ResultSet resultSet;
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
		ResultSet knnSearch(unsigned k, const Point& point) const
		{
			struct QueueEntry {
				union {
					N * node;
					Id id;
				};
				unsigned depth;
				float distance;

				QueueEntry(Id id, unsigned depth, float d)
					: id(id), depth(depth), distance(d) {};

				QueueEntry(N * node, unsigned depth, float d)
					: node(node), depth(depth), distance(d) {};
			};

			ResultSet results;

			std::priority_queue<
					QueueEntry,
					std::vector<QueueEntry>,
					std::function<bool(const QueueEntry&, const QueueEntry&)>
				> queue (
					[&](const QueueEntry& a, const QueueEntry& b) -> bool {
						if (
							a.distance == b.distance &&
							a.depth == height &&
							b.depth == height
						) {
								return a.id > b.id;
						}

						return a.distance > b.distance;
					}
				);

			queue.emplace(root, 0, 0.0f);

			while (!queue.empty() && results.size() < k) {
				const QueueEntry& top = queue.top();
				unsigned depth = top.depth;

				if (depth == height) {
					results.push_back(top.id);
					queue.pop();
					continue;
				}

				E * entries = top.node->entries;
				auto  nEntries = top.node->nEntries;

				queue.pop();

				if (depth == height - 1) {
					for (E * e = entries; e != entries + nEntries; ++e) {
						queue.emplace(
								e->id,
								height,
								e->mbr.distance2(point)
							);
					}
				} else {
					for (E * e = entries; e != entries + nEntries; ++e) {
						queue.emplace(
								e->node,
								depth + 1,
								e->mbr.distance2(point)
							);
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
			for (unsigned i = 0; i < height - 1; i++) { // while (node not leaf)
				struct {
					unsigned index = 0;
					float enlargement = std::numeric_limits<float>::infinity();
				} best;

				if (node->nEntries < 1) {
					throw std::logic_error("Trying to find child of empty node");
				}

				// Find best child
				for (unsigned j = 0; j < node->nEntries; j++) {
					M& mbr = node->entries[j].mbr;
					float enlargement = mbr.enlargement(point);

					if (enlargement < best.enlargement) {
						best.enlargement = enlargement;
						best.index = j;
					}
				}

				// Enlarge the entry MBR
				node->entries[best.index].mbr += M(point);

				E& entry = node->entries[best.index];
				node = entry.node;
				path.push_back(&entry);
			}

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
			std::reverse(path.begin(), path.end());
			auto top = path.begin();

			while (top != path.end() && (*(top))->node->isFull()) {
				e = (*top)->split(e, allocate());
				top++;
			}

			if (top != path.end()) {
				(*top)->node->add(e);
				return;
			}

			if (root->isFull()) {
				E oldRoot (root, M());
				root = allocate();
				root->add(oldRoot);
				root->add(root->entries[0].split(e, allocate()));
				height += 1;
			} else {
				root->add(e);
			}
		};


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
