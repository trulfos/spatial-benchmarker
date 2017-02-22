#pragma once
#include <numeric>
#include "Split.hpp"

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
		GoalFunction(M mbr)
		{
			maxPerimeter = 2 * mbr.perimeter();
		}

		/**
		 * Evaluate the goal function for the given split.
		 */
		double operator()(
				const Split<E>& split,
			   	bool useVolume = true
		) {

			// Sum up MBRs (again)
			auto mbrs = split.getMbrs();

			// Return overlap (if any)
			if (mbrs[0].intersects(mbrs[1])) {
				const M intersection = mbrs[0].intersection(mbrs[1]);

				double overlap = useVolume ?
					intersection.volume() : intersection.perimeter();

				if (overlap != 0.0) {
					return overlap;
				}
			}

			// Otherwise, use (negative) perimeter
			return split.perimeter() - maxPerimeter;
		}

	private:
		double maxPerimeter;
};

}
