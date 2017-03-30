#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include "QuadraticRtree.hpp"
#include "Node.hpp"
#include "Entry.hpp"

namespace Rtree
{

/**
 * R-tree with Greene split strategy.
 *
 * @tparam D Dimenson
 * @tparam N Node type
 * @tparam m Minimum node fill grade
 */
template<unsigned D, unsigned C, unsigned m>
class GreeneRtree : public QuadraticRtree<D, C, m> //TODO: Not logical inheritance
{
	protected:
		using E = typename QuadraticRtree<D, C, m>::E;
		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The first entry (with children)
		 * @param b The second entry (with children)
		 */
		void redistribute(E& a, E& b, unsigned) override
		{
			// Contruct buffer with all entries
			std::vector<E> entries (
					a.begin(), a.end()
				);

			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);


			// Choose the two seeds by checking all combinations
			typename std::vector<E>::iterator seeds[2];
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
			auto enclosing = a.getMbr() + b.getMbr();
			float maxSeparation = -1.0f;
			unsigned splitDimension = E::dimension;

			for (unsigned d = 0; d < E::dimension; ++d) {

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
			assert(splitDimension < E::dimension);


			// Sort by low value
			std::sort(
					entries.begin(),
					entries.end(),
					[&](const E& a, const E& b) {
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
