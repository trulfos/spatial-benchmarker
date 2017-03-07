#pragma once
#include <cassert>
#include <vector>
#include "BasicRtree.hpp"
#include "Node.hpp"
#include "Entry.hpp"
#include "Algorithm.hpp"
#include "QuadraticSeeds.hpp"
#include <cmath>
#include <iostream>

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
class QuadraticRtree : public BasicRtree<Node<D, C, Entry>, m>
{
	protected:
		using E = typename Node<D, C, Entry>::Entry;


		/**
		 * Find the child requiring the least MBR enlargement to include the
		 * given MBR.
		 *
		 * @param parent Parent entry from which to select a child
		 * @param newEntry New entry to include
		 * @return Entry requiring the least enlargement to include mbr
		 */
		E& chooseSubtree(E& parent, const E& newEntry) override
		{
			return *argmin(
					parent.begin(), parent.end(),
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
		void redistribute(E& a, E& b, unsigned level) override
		{
			// Contruct buffer with all entries
			std::vector<E> entries (a.begin(), a.end());
			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);


			// Generate seeds
			using EIt = typename decltype(entries)::iterator;
			QuadraticSeeds<EIt> seeds (entries.begin(), entries.end());


			/*
			if (seeds.second == entries.end() - 1) {
				std::cout << seeds.first->mbr.waste(seeds.second->mbr) << std::endl;
			}
			*/

			// Assign and remove seeds from entries
			a = {*seeds.first};
			b = {*seeds.second};

			assert(seeds.first < seeds.second);

			std::iter_swap(seeds.second, entries.end() - 1);
			std::iter_swap(seeds.first, entries.end() - 2);

			entries.resize(entries.size() - 2);

			// Add entries
			for (auto entry = entries.begin(); entry != entries.end(); ++entry) {

				// Do we have to add the remainding to one side?
				if (a.node->nEntries >= entries.size() + 2 - m) {
					b.add(*entry);
					continue;
				}

				if (b.node->nEntries >= entries.size() + 2 - m) {
					a.add(*entry);
					continue;
				}

				// Select the one waisting the most space if placed wrong
				auto selected = argmin(
						entry, entries.end(),
						[&](const E& entry) {
							return -std::fabs(
									a.mbr.enlargement(entry.mbr)
									- b.mbr.enlargement(entry.mbr)
								);
						}
					);

				// "Remove" the entry from further consideration
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
		};
};

}
