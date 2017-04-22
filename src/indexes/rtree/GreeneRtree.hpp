#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include "QuadraticRtree.hpp"
#include "Entry.hpp"

namespace Rtree
{

/**
 * R-tree with Greene split strategy.
 *
 * @tparam N Node type
 * @tparam m Minimum node fill grade
 */
template<class Node, unsigned m>
class GreeneRtree : public QuadraticRtree<Node, m> //TODO: Not logical inheritance
{
	protected:
		using N = Node;
		using Mbr = typename N::Mbr;

		static constexpr unsigned D = Mbr::dimension;

		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The first entry (with children)
		 * @param b The second entry (with children)
		 */
		void redistribute(
				BaseEntry<N>& original,
			   	BaseEntry<N>& newEntry,
			   	unsigned
			) override
		{
			N& a = original.getNode();
			N& b = newEntry.getNode();
			Mbr enclosing = original.getMbr() + newEntry.getMbr();

			// Contruct buffer with all entries
			std::vector<Entry<N>> entries (
					a.begin(), a.end()
				);

			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);


			// Choose the two seeds by checking all combinations
			typename decltype(entries)::iterator seeds[2];
			float maxDist = -1.0f;

			for (auto i = entries.begin(); i != entries.end(); ++i) {
				for (auto j = i + 1; j != entries.end(); ++j) {

					float dist = i->getMbr().distance2(j->getMbr());

					if (maxDist < dist) {
						seeds[0] = i;
						seeds[1] = j;
						maxDist = dist;
					}
				}
			}

			assert(maxDist >= 0.0f);

			// Determine split dimension
			float maxSeparation = -1.0f;
			unsigned splitDimension = D;

			for (unsigned d = 0; d < D; ++d) {

				// Separation, normalized by enclosing size
				float separation = seeds[0]
					->getMbr()
					.distanceAlong(d, seeds[1]->getMbr())
						/ (enclosing.getTop()[d] - enclosing.getBottom()[d]);

				if (maxSeparation < separation) {
					splitDimension = d;
					maxSeparation = separation;
				}
			}

			// We should have found a dimension
			assert(splitDimension < D);


			// Sort by low value
			std::sort(
					entries.begin(),
					entries.end(),
					[&](const Entry<N>& a, const Entry<N>& b) {
						return a.getMbr().getBottom()[splitDimension]
							< b.getMbr().getBottom()[splitDimension];
					}
				);

			// Distribute the remaining entries
			auto middle = entries.begin() + (entries.size() / 2 - 1);

			a.assign(
					entries.begin(),
					middle
				);

			b.assign(
					middle,
					entries.end()
				);
		};
};

}
