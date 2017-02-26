#pragma once
#include <cassert>
#include <vector>
#include "Rtree.hpp"
#include "Node.hpp"
#include "HilbertEntry.hpp"
#include "common/Algorithm.hpp"

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
class HilbertRtree : public Rtree<Node<D, C, HilbertEntry>>
{
	public:

		using N = Node<D, C, HilbertEntry>;
		using E = typename N::Entry;
		using EIt = typename E::iterator;


		void setBounds(const Box& bounds) override
		{
			this->bounds = bounds;
		}

		/**
		 * Insert an entry in the tree.
		 *
		 * @param object DataObject to insert
		 */
		void insert(const DataObject& object) override
		{
			E entry (object, bounds);
			E rootEntry (this->getRoot(), typename E::M());
			std::vector<EIt> path {&rootEntry};


			// Find leaf node
			for (unsigned i = 0; i < this->getHeight() - 1; i++) {
				assert(std::is_sorted(
						path.back()->begin(), path.back()->end(),
						[](const E& a, const E& b) {
							return a.getHilbertValue() < b.getHilbertValue();
						}
					));

				E& e = chooseSubtree(*path.back(), entry);
				e.include(entry);
				path.push_back(&e);
			}

			// Split nodes bottom-up as long as necessary
			E e = entry;
			auto top = path.rbegin();

			while (top != path.rend() && (*top)->node->isFull()) {

				if (top + 1 == path.rend()) {

					// Split root
					e = E(this->allocateNode(), {e});

					E newRoot (this->allocateNode(), {**top, e});
					redistribute(newRoot.begin(), newRoot.end());

					this->addLevel(newRoot.node);
					return;

				}

				E& parent = **(top + 1);
				EIt destination = *top;

				// Calculate first (left) position to check
				EIt start = std::max(
						destination - (
								s - 1 - std::min(
									s / 2,
									unsigned(parent.end() - destination) - 1
								)
							),
						parent.begin()
					);

				EIt end = std::min(start + s, parent.end());

				assert(end <= parent.end() && end > parent.begin());
				assert(start >= parent.begin() && start < parent.end());

				// Find closest position with available space
				EIt newDest = argmin(
						start, end,
						[&](const E& entry) {
							if (entry.node->isFull()) {
								return std::numeric_limits<unsigned>::max();
							}

							//TODO: This is a bit disgusting
							int distance = destination - &entry;
							return 2u * std::abs(distance) + (distance > 0 ? 1u : 0u);
						}
					);

				assert(destination >= start && destination < end);
				assert(newDest >= start && newDest < end);

				// Insert and redistribute?
				if (!newDest->node->isFull()) {
					newDest->add(e);

					if (newDest < destination) {
						std::swap(newDest, destination);
					}

					assert(destination < newDest);

					redistribute(destination, newDest + 1);
					return;
				}

				// No space left - create a new node
				e = E(this->allocateNode(), {e});
				redistribute(start, end, e);

				top++;
			}

			(*top)->add(e);
		};


	protected:

		Box bounds;

		/**
		 * Chooses subtree by comparing hilbert values.
		 * The first entry with a hilbert value greater or equal to the new
		 * entry is selected.
		 *
		 * @param parent Parent from which a child should be selected
		 * @return Selected child entry (subtree)
		 */
		E& chooseSubtree(E& parent, const E& entry)
		{

			// Binary search through the children
			auto it = std::upper_bound(
					parent.begin(), parent.end(),
					entry.getHilbertValue(),
					[](const std::uint64_t hv, const E& e) {
						return hv < e.getHilbertValue();
					}
				);

			// If no element is larger, choose the largest
			return *(it == parent.end() ? parent.end() - 1 : it);
		};


		/**
		 * Calls redistribute with the given range and the extra entry at the
		 * end.
		 *
		 * @param start Iterator to first entry
		 * @param end Iterator to last entry
		 * @param extra Extra entry to include
		 */
		void redistribute(EIt start, EIt end, E& extra)
		{
			// Create list of pointers to all entries
			std::vector<E *> neighbors;

			std::transform(
					start, end,
					std::back_inserter(neighbors),
					[](E& a) { return &a; }
				);

			neighbors.push_back(&extra);

			// Call redistribute and collapse double references to hide the
			// illusion
			redistribute(
					makeDerefIt(neighbors.begin()),
					makeDerefIt(neighbors.end())
				);
		}


		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param start Iterator to first entry
		 * @param end Iterator to last entry
		 */
		template<class It>
		void redistribute(It start, It end)
		{
			// Contruct buffer with all entries
			std::vector<E> entries;

			for (It i = start; i != end; ++i) {
				entries.insert(
						entries.end(),
						i->begin(), i->end()
					);
			}

			// Efficient coding... (and bad performance)
			// This is necessary until we know the order of the parents and
			// as long as we insert the new entry "randomly".
			std::sort(
					entries.begin(), entries.end(),
					[](const E& a, const E& b) {
						return a.getHilbertValue() < b.getHilbertValue();
					}
				);


			// Distribute the entries
			unsigned nParents = end - start;
			unsigned base = entries.size() / nParents;
			unsigned leftover = entries.size() - (base * nParents);
			auto it = entries.begin();

			for (It i = start; i != end; ++i) {

				auto end = it + base;

				// Distribute left-overs to first nodes
				if (leftover) {
					end++;
					leftover--;
				}

				// Copy the entries
				i->assign(it, end);

				// Prepare for next round
				it = end;
			}
		};
};

}
