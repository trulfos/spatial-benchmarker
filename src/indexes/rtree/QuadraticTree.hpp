#pragma once
#include <cassert>
#include <vector>
#include "Rtree.hpp"
#include "Node.hpp"
#include "Entry.hpp"
#include "common/Algorithm.hpp"
#include "CombinationsView.hpp"
#include <cmath>

namespace Rtree
{

/**
 * R-tree with quadratic split as suggested by Guttman.
 *
 * @tparam D Dimenson
 * @tparam N Node type
 * @tparam m Minimum node fill grade
 */
template<unsigned D, unsigned C, unsigned m>
class QuadraticRtree : public Rtree<Node<D, C, Entry>>
{
	public:

		using N = Node<D, C, Entry>;
		using E = typename N::Entry;
		using M = typename E::M;


		/**
		 * Insert an entry in the tree.
		 *
		 * @param object DataObject to insert
		 */
		void insert(const DataObject& object) override
		{
			E entry (object);
			E rootEntry (this->getRoot(), M());
			std::vector<E *> path {&rootEntry};

			// Find leaf node
			for (unsigned i = 0; i < this->getHeight() - 1; i++) {
				E& e = leastVolumeEnlargement(*path.back(), entry);
				e.mbr += entry.mbr;
				path.push_back(&e);
			}

			// Split nodes bottom-up as long as necessary
			auto top = path.rbegin();

			while (top != path.rend() && (*top)->node->isFull()) {
				entry = E(this->allocateNode(), {entry});
				redistribute(**top, entry);
				++top;
			}

			// Split root?
			if (top == path.rend()) {
				E newRoot (this->allocateNode(), {**path.begin(), entry});
				this->addLevel(newRoot.node);
			} else {
				(*top)->add(entry);
			}
		};


	protected:


		/**
		 * Find the child requiring the least MBR enlargement to include the
		 * given MBR.
		 *
		 * @param parent Parent entry from which to select a child
		 * @param newEntry New entry to include
		 * @return Entry requiring the least enlargement to include mbr
		 */
		template<class E>
		E& leastVolumeEnlargement(E& parent, const E& newEntry)
		{
			return *argmin(
					parent.begin(),
					parent.end(),
					[&](const E& entry) {
						return std::make_tuple(
								entry.mbr.enlargement(newEntry.mbr),
								entry.mbr.volume()
							);
					}
				);
		}



		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The first entry (with children)
		 * @param b The second entry (with children)
		 */
		template<class E>
		void redistribute(E& a, E& b)
		{
			// Contruct buffer with all entries
			std::vector<E> entries (a.begin(), a.end());
			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);

			// Choose the two seeds by checking all combinations
			auto combinations = makeCombinationsView(
					entries.begin(), entries.end()
				);

			auto seeds = argmin(
					combinations.begin(), combinations.end(),
					[](std::pair<E&, E&> pair) {
						M mbrA = pair.first.mbr;
						M mbrB = pair.second.mbr;

						return -(
								(mbrA + mbrB).volume() - (
										mbrA.volume() + mbrB.volume()
									)
							);
					}
				).getIterators();


			// Assign and remove seeds from entries
			a = {*seeds.first};
			b = {*seeds.second};

			assert(seeds.first < seeds.second);

			if (seeds.first != entries.end() - 2) {
				std::iter_swap(seeds.second, entries.end() - 1);
				std::iter_swap(seeds.first, entries.end() - 2);
			}

			entries.resize(entries.size() - 2);

			// Add entries
			for (auto entry = entries.begin(); entry != entries.end(); ++entry) {

				// Do we have to add all to one side?
				if (a.node->nEntries >= C - m) {
					b.add(*entry);
					continue;
				}

				if (b.node->nEntries >= C - m) {
					a.add(*entry);
					continue;
				}

				// Select the one waisting the most space
				auto selected = argmin(
						entry, entries.end(),
						[&](const E& entry) {
							return -std::fabs(
									a.mbr.enlargement(entry.mbr)
									- b.mbr.enlargement(entry.mbr)
								);
						}
					);

				std::iter_swap(entry, selected);

				if (
						std::make_tuple(
								a.mbr.enlargement(entry->mbr),
								a.mbr.volume(),
								a.node->nEntries
							) > std::make_tuple(
								b.mbr.enlargement(entry->mbr),
								b.mbr.volume(),
								b.node->nEntries
							)
				) {
					b.add(*entry);
				} else {
					a.add(*entry);
				}
			}

			assert(a.node->nEntries >= m);
			assert(b.node->nEntries >= m);
		};
};

}
