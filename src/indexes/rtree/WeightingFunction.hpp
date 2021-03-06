#pragma once
#include <cassert>
#include <cmath>

namespace Rtree
{

/**
 * Weighting function used by the revised R*-tree.
 *
 * Note that some calculations have been reordered to allow precalculating most
 * of the values once the dimension is known.
 */
template<class N, unsigned m>
class WeightingFunction
{
	using Mbr = typename N::Mbr;


	static constexpr unsigned capacity = N::capacity;
	static constexpr double s = 0.5f;
	static constexpr double shift = exp(-1.0f / (s * s));
	static constexpr double scale = 1.0f / (1 - shift);

	static_assert(2 * m <= capacity, "m must be less than or equal to M/2");

public:

	/**
	 * Construct a new weighting function based on the given entry.
	 *
	 * @param Parent to base calculations on
	 */
	template<class E>
	WeightingFunction(const E& parent) :
		current(parent.getMbr()),
		original(parent.getPlugin().originalMbr(parent)),
		dimension(1)
	{
		setDimension(0);
	}


	/**
	 * Set the dimension and precalculate relevant results.
	 *
	 * @param d Dimension
	 */
	void setDimension(unsigned d)
	{
		assert(d < Mbr::dimension);

		// Skip if correct dimension is already set
		if (dimension == d) {
			return;
		}

		dimension = d;

		const double width = current.getTop()[d] - current.getBottom()[d];

		// Recalculate cached values
		double asym = width == 0.0f ? 0.0f : (
					(current.getTop()[d] + current.getBottom()[d])
					- (original.getTop()[d] + original.getBottom()[d])
				) / width;

		assert(asym <= 1.0f && asym >= -1.0f);

		preShift = 0.5f * (capacity + 1.0f + (capacity + 1.0f - 2.0f * m) * asym);
		preScale = 2.0f / (s * (capacity + 1 + (capacity + 1 - 2 * m) * fabs(asym)));
	}


	/**
	 * Calculate the value of the weighting function.
	 *
	 * @param i Split index
	 */
	double operator()(unsigned i) const
	{
		double e = preScale * (i - preShift);
		double r = scale * (exp(-e * e) - shift);

		assert (r >= 0.0);

		return r;
	}

private:
	Mbr current;
	Mbr original;

	// Used to cache the below result
	unsigned dimension = 0;

	// Temporary results
	double preShift;
	double preScale;


};

};
