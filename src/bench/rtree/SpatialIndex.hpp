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

namespace Rtree
{


/**
 * R-tree algorithms without optimizations.
 *
 * Does not consider disk pages, as this is memory resident anyway.
 *
 * Template parameters:
 * D is the dimensionality
 * C is the maximal number of entries in each node
 */
template <unsigned D, unsigned C>
class SpatialIndex : public ::SpatialIndex
{
	using M = Mbr<D>;
	using N = Node<D, C>;
	using E = Entry<D, C>;

	public:
		~SpatialIndex()
		{
			//TODO: Delete tree
		};

		SpatialIndex(const DataSet& dataSet)
		{
			if (dataSet.empty()) {
				return;
			}

			root = new N();
			height = 1;

			for (auto& object : dataSet) {
				insert(object);
			}
		};


	protected:
		ResultSet rangeSearch(const AxisAlignedBox& box) const
		{
			ResultSet resultSet;

			//TODO: Iterate the other way

			std::stack<std::pair<E *, unsigned>> path;
			path.emplace(root->entries, root->nEntries);

			while (!path.empty()) {
				auto& top = path.top();

				if (top.second == 0) {
					path.pop();
					continue;
				}

				top.second -= 1;
				E& entry = top.first[top.second];

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

		ResultSet knnSearch(unsigned k, const Point& point) const
		{
			ResultSet results;

			return results;
		};


	private:
		using Id = DataObject::Id;

		unsigned height;
		N * root;

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


		void split(std::vector<E *> path, const E& newEntry)
		{
			E e = newEntry;
			std::reverse(path.begin(), path.end());
			auto top = path.begin();

			// Split from bottom and upwards
			while (top != path.end() && (*(top))->node->isFull()) {
				e = (*top)->split(e);
				top++;
			}

			if (top == path.end()) {
				if (root->isFull()) {
					E oldRoot (root, M());
					root = new N();
					root->add(oldRoot);
					root->add(root->entries[0].split(e));
					height += 1;
				} else {
					root->add(e);
				}
			} else {
				(*(top - 1))->node->add(e);
			}
		};
};

}
