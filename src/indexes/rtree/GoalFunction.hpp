#pragma once
#include <numeric>
#include "Split.hpp"
#include "Range.hpp"

namespace Rtree
{

/**
 * The goal function of the RR*-tree.
 */
class GoalFunction
{

	public:

		/**
		 * Create a new goal function based on the given entries.
		 *
		 * @param mbr MBR of all entries considered
		 */
		template<class M>
		explicit GoalFunction(const M& mbr)
		{
			double minProjection = min_value(
					makeRangeIt(0u), makeRangeIt(M::dimension),
					[&](unsigned d) {
						return mbr.getTop()[d] - mbr.getBottom()[d];
					}
				);

			maxPerimeter = 2 * mbr.perimeter() - minProjection;
		}

		/**
		 * Evaluate the goal function for the given split.
		 */
		template<class S>
		double operator()(const S& split, bool useVolume) {

			auto mbrs = split.getMbrs();
			using M = typename S::Mbr;

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

		double getMaxPerimeter()
		{
			return maxPerimeter;
		}

	private:
		double maxPerimeter;
};

}
