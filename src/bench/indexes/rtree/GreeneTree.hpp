#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include "QuadraticTree.hpp"
#include "Node.hpp"
#include "Entry.hpp"

namespace Rtree
{

/**
 * R-tree with Greene split strategy.
 *
 * @tparam D Dimenson
 * @tparam N Node type
 */
template<unsigned D, unsigned C>
class GreeneRtree : public QuadraticRtree<D, C> //TODO: Not logical inheritance
{
	public:
		GreeneRtree(LazyDataSet& dataSet) : QuadraticRtree<D, C>(dataSet)
		{
		};

	protected:
		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The first entry (with children)
		 * @param b The second entry (with children)
		 */
		template<class E>
		static void redistribute(E& a, E& b)
		{
			// Contruct buffer with all entries
			std::vector<E> entries (
					a.node->begin(),
					a.node->end()
				);

			entries.insert(
					entries.end(),
					b.node->begin(),
					b.node->end()
				);


			// Choose the two seeds by checking all combinations
			typename std::vector<E>::iterator seeds[2];
			float maxDist = -1.0f;

			for (auto i = entries.begin(); i != entries.end(); ++i) {
				for (auto j = i + 1; j != entries.end(); ++j) {

					float dist = i->mbr.distance2(j->mbr);

					if (maxDist < dist) {
						seeds[0] = i;
						seeds[1] = j;
						maxDist = dist;
					}
				}
			}

			assert(maxDist >= 0.0f);

			// Determine split dimension
			auto enclosing = a.mbr + b.mbr;
			float maxSeparation = -1.0f;
			unsigned splitDimension = E::dimension;

			for (unsigned d = 0; d < E::dimension; ++d) {

				// Separation, normalized by enclosing size
				float separation = seeds[0]->mbr.distanceAlong(d, seeds[1]->mbr)
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
						return a.mbr.getBottom()[splitDimension]
							< b.mbr.getBottom()[splitDimension];
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
