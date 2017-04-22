#pragma once
#include <cassert>
#include <vector>
#include "Rtree.hpp"
#include "Node.hpp"
#include "HilbertEntryPlugin.hpp"
#include "Algorithm.hpp"

namespace Rtree
{

/**
 * R-tree using the Hilbert order to split nodes in a fashion very similar to
 * B-trees.
 *
 * @tparam D Dimenson
 * @tparam N Node type
 * @tparam s Node split strategy s:(s+1)
 */
template<unsigned D, unsigned C, unsigned s>
class HilbertRtree : public Rtree<Node<D, C, HilbertEntryPlugin>>
{
	using N = Node<D, C, HilbertEntryPlugin>;
	using NIt = typename N::iterator;
	using Base = Rtree<N>;

	struct HilbertCompare
	{
		bool operator()(const Entry<N>& a, const Entry<N>& b) const
		{
			return a.getPlugin().getHilbertValue()
				< b.getPlugin().getHilbertValue();
		}
	};


	public:

		using Base::addLevel;
		using Base::getHeight;
		using Base::getRoot;


		/**
		 * Create a new hilbert entry with the given bounds.
		 *
		 * The bounds are used to normalize the coordinates such that the
		 * hilbert encoding becomes more efficient.
		 *
		 * @param bounds Data domain boundaries
		 */
		HilbertRtree(const Box& bounds) : bounds(bounds)
		{
		}


		/**
		 * Insert an entry in the tree.
		 *
		 * @param object DataObject to insert
		 */
		void insert(const DataObject& object) override
		{
			Entry<N> entry (object, bounds);

			// No nodes - set entry as root
			if (getHeight() == 0) {
				addLevel(entry);
				return;
			}

			// Single entry - add new root
			if (getHeight() == 1) {
				addLevel(
						Entry<N>(new N({getRoot(), entry}))
					);

				N& root = getRoot().getNode();
				std::sort(root.begin(), root.end(), HilbertCompare());

				return;
			}

			getRoot().include(entry);


			if (getHeight() == 2) {
				if (getRoot().getNode().isFull()) {
					splitRoot(entry);
					return;
				}

				N& root = getRoot().getNode();
				root.add(entry);
				std::sort(root.begin(), root.end(), HilbertCompare());
				return;
			}

			std::vector<NIt> path {
					chooseSubtree(getRoot().getNode(), entry)
				};

			path.back()->include(entry);

			// Find leaf node
			for (unsigned i = 0; i < getHeight() - 3; i++) {
				path.push_back(
						chooseSubtree(path.back()->getNode(), entry)
					);
				path.back()->include(entry);
			}

			// Share entries or split nodes bottom-up as long as necessary
			auto top = path.rbegin();

			while (top != path.rend() && (*top)->getNode().isFull()) {

				NIt destination = *top;
				std::pair<NIt, NIt> range = calculateNeighborRange(destination);
				NIt newDest = locateNeighbor(destination, range);

				// Insert and redistribute?
				if (newDest != destination.getContainingNode().end()) {
					newDest->getNode().add(entry);

					if (newDest < destination) {
						std::swap(newDest, destination);
					}

					redistribute(destination, newDest + 1);
					return;
				}

				// No space left - create a new node
				entry = Entry<N>(new N({entry}));
				redistribute(range.first, range.second, &entry);

				top++;
			}

			// Split root?
			if (top == path.rend()) {
				N& node = getRoot().getNode();

				if (node.isFull()) {
					splitRoot(entry);
				} else {
					node.add(entry);
					std::sort(node.begin(), node.end(), HilbertCompare());
				}
				return;
			}

			// Regular insertion
			N& node = (*top)->getNode();
			node.add(entry);
			std::sort(node.begin(), node.end(), HilbertCompare());
		};


	protected:

		Box bounds;


		/**
		 * Split the root node.
		 *
		 * A new root is created with two children containing the original
		 * children of the root node and the given entry.
		 *
		 * @param entry Entry to include as child of old root
		 */
		void splitRoot(Entry<N> entry)
		{
			entry = Entry<N>(new N({entry}));

			Entry<N> newRoot (new N({getRoot(), entry}));
			redistribute(
					newRoot.getNode().begin(),
					newRoot.getNode().end()
				);

			addLevel(newRoot);
		}



		/**
		 * Find the range of neighbors for the given reference entry.
		 *
		 * This range is normally the (1+s)/2 nodes to the left and right of the
		 * entry, but may be offset when the reference is close to the beginning
		 * or end of the node.
		 *
		 * @param reference Center of range to calculate
		 * @return Pair of iterators to the calculated range
		 */
		std::pair<NIt, NIt> calculateNeighborRange(NIt reference)
		{
			N& parent = reference.getContainingNode();

			// Calculate first (left) position to check
			NIt start = parent.begin() + std::max(
					(reference - parent.begin()) - (
							int(s) - 1 - std::min(
								int(s) / 2,
								parent.end() - reference - 1
							)
						),
					0
				);

			NIt end = std::min(start + s, parent.end());

			assert(end <= parent.end() && end > parent.begin());
			assert(start >= parent.begin() && start < parent.end());
			assert(reference >= start && reference < end);

			return std::make_pair(start, end);
		}


		/**
		 * Locates the nearest node (with respect to referenc) in the range with
		 * available space.
		 *
		 * @param reference Center of the neighbors
		 * @return Iterator to node with available space or end if none is found
		 */
		NIt locateNeighbor(NIt reference, std::pair<NIt, NIt> range)
		{
			int offset = 1;
			short nBoundsHit = 0;
			unsigned i = 0;

			while (nBoundsHit < 2 && i < s) {
				if (
					(offset < 0 && (reference - range.first) >= -offset) ||
					(offset > 0 && (range.second - reference) > offset)
				) {
					NIt candidate = reference + offset;
					if (!candidate->getNode().isFull()) {
						return candidate;
					}

					nBoundsHit = 0;
					i++;
				} else {
					nBoundsHit++;
				}

				offset = -offset + (offset < 0);
			}

			return reference.getContainingNode().end();
		}


		/**
		 * Chooses subtree by comparing hilbert values.
		 * The first entry with a hilbert value greater or equal to the new
		 * entry is selected.
		 *
		 * @param parent Parent from which a child should be selected
		 * @return Selected child entry (subtree)
		 */
		NIt chooseSubtree(N& parent, const Entry<N>& entry)
		{

			// Binary search through the children
			auto it = std::upper_bound(
					parent.begin(), parent.end(),
					entry.getPlugin().getHilbertValue(),
					[](const std::uint64_t hv, typename N::reference e) {
						return hv < e.getPlugin().getHilbertValue();
					}
				);

			// If no element is larger, choose the largest
			return it == parent.end() ? parent.end() - 1 : it;
		};


		/**
		 * Calls redistribute with the given range and the extra entry at the
		 * end.
		 *
		 * @param start Iterator to first entry
		 * @param end Iterator to last entry
		 * @param extra Extra entry to include (optional)
		 */
		void redistribute(NIt start, NIt end, Entry<N> * extra = nullptr)
		{
			//std::cout << "Redistribute" << std::endl;
			// Create list of pointers to all nodes
			std::vector<N *> nodes;

			std::transform(
					start, end,
					std::back_inserter(nodes),
					[](typename N::reference a) { return &a.getNode(); }
				);

			if (extra != nullptr) {
				nodes.push_back(&extra->getNode());
			}

			// Contruct buffer with all entries
			std::vector<Entry<N>> entries;

			for (N * node : nodes) {
				entries.insert(
						entries.end(),
						node->begin(), node->end()
					);
			}

			// Efficient coding (and bad performance)
			// This is necessary until we know the order of the parents and
			// as long as we insert the new entry "randomly".
			std::sort(
					entries.begin(), entries.end(),
					[](const Entry<N>& a, const Entry<N>& b) {
						return a.getPlugin().getHilbertValue()
							< b.getPlugin().getHilbertValue();
					}
				);


			// Distribute the entries
			unsigned nParents = nodes.size();
			unsigned base = entries.size() / nParents;
			unsigned leftover = entries.size() - (base * nParents);
			auto it = entries.begin();

			for (N * node : nodes) {

				auto end = it + base;

				// Distribute left-overs to first nodes
				if (leftover) {
					end++;
					leftover--;
				}

				// Copy the entries
				node->assign(it, end);

				// Prepare for next round
				it = end;
			}


			// Recalculate MBRs and plugins
			while (start != end) {
				start->recalculate();
				++start;
			}

			if (extra != nullptr) {
				extra->recalculate();
			}
			
		};
};

}
