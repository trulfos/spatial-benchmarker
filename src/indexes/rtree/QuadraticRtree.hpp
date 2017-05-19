#pragma once
#include <cassert>
#include <vector>
#include "BasicRtree.hpp"
#include "DefaultNode.hpp"
#include "Entry.hpp"
#include "Algorithm.hpp"
#include "QuadraticSeeds.hpp"
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
template<class Node, unsigned m>
class QuadraticRtree : public BasicRtree<Node, m>
{
	protected:
		using N = Node;
		using M = typename N::Mbr;


		/**
		 * Find the child requiring the least MBR enlargement to include the
		 * given MBR.
		 *
		 * @param parent Parent entry from which to select a child
		 * @param newEntry New entry to include
		 * @return Entry requiring the least enlargement to include mbr
		 */
		typename N::iterator chooseSubtree(
				Entry<N> parent,
				const Entry<N>& newEntry
			) override
		{
			N& node = parent.getLink().getNode();

			return argmin(
					node.begin(), node.end(),
					[&](typename N::reference& entry) {
						M mbr = entry.getMbr();

						return std::make_tuple(
								mbr.delta(
										&M::volume,
										newEntry.getMbr()
									),
								mbr.volume()
							);
					}
				);
		}


		void redistribute(
				Entry<N> original,
				Entry<N> newEntry,
				unsigned
			) override
		{
			N& a = original.getNode();
			N& b = newEntry.getNode();

			// Contruct buffer with all entries
			std::vector<Entry<N>> entries (
					a.begin(), a.end()
				);

			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);


			// Generate seeds
			using EIt = typename decltype(entries)::iterator;
			QuadraticSeeds<EIt> seeds (entries.begin(), entries.end());


			// Assign and remove seeds from entries
			a = {*seeds.first};
			b = {*seeds.second};

			M mbrA = seeds.first->getMbr();
			M mbrB = seeds.second->getMbr();

			assert(seeds.first < seeds.second);

			std::iter_swap(seeds.second, entries.end() - 1);
			std::iter_swap(seeds.first, entries.end() - 2);

			entries.resize(entries.size() - 2);

			// Add entries
			for (auto entry = entries.begin(); entry != entries.end(); ++entry) {

				// Do we have to add the remaining to one side?
				if (a.getSize() == entries.size() + 2 - m) {
					b.add(entry, entries.end());
					return;
				}

				if (b.getSize() == entries.size() + 2 - m) {
					a.add(entry, entries.end());
					return;
				}

				// Select the one waisting the most space if placed wrong
				auto selected = argmin(
						entry, entries.end(),
						[&](const Entry<N>& entry) {
							return -std::fabs(
									mbrA.delta(&M::volume, entry.getMbr())
									- mbrB.delta(&M::volume, entry.getMbr())
								);
						}
					);

				// "Remove" the entry from further consideration
				std::iter_swap(entry, selected);

				// Add entry to correct node
				if (
						std::make_tuple(
								mbrA.delta(&M::volume, entry->getMbr()),
								mbrA.volume(),
								a.getSize()
							) > std::make_tuple(
								mbrB.delta(&M::volume, entry->getMbr()),
								mbrB.volume(),
								b.getSize()
							)
				) {
					b.add(*entry);
					mbrB += entry->getMbr();
				} else {
					a.add(*entry);
					mbrA += entry->getMbr();
				}
			}
		};
};

}
