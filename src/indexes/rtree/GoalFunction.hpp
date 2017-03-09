#pragma once
#include <numeric>
#include "Split.hpp"
#include "Algorithm.hpp"

namespace Rtree
{

/**
 * This si the goal function of the RR*-tree.
 */
template<class E>
class GoalFunction
{

	public:

		using M = typename E::M;

		/**
		 * Create a new goal function based on the given entries.
		 *
		 * @param mbr MBR of all entries considered
		 */
		GoalFunction(const M& mbr)
		{
			double minProjection = min_value(
					makeRangeIt(0u), makeRangeIt(E::dimension),
					[&](unsigned d) {
						return mbr.getTop()[d] - mbr.getBottom()[d];
					}
				);

			maxPerimeter = 2 * mbr.perimeter() - minProjection;
		}

		/**
		 * Evaluate the goal function for the given split.
		 */
		double operator()(const Split<E>& split, bool useVolume) {
			auto mbrs = split.getMbrs();

			// Return overlap (if any)
			double overlap = mbrs[0].overlap(
					useVolume ? &M::volume : &M::perimeter,
					mbrs[1]
				);

			if (overlap != 0.0) {
				assert(overlap > 0.0);
				return overlap;
			}

			// Otherwise, use (shifted) perimeter
			assert(split.perimeter() <= maxPerimeter);
			return split.perimeter() - maxPerimeter;
		}

	private:
		double maxPerimeter;
};

}
