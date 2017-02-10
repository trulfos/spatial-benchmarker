#pragma once
#include <cassert>
#include <cmath>
#include "common/Coordinate.hpp"

namespace Rtree
{

/**
 * Weighting function used by the revised R*-tree.
 *
 * Note that some calculations have been reordered to allow precalculating most
 * of the values once the dimension is known.
 */
template<class E, unsigned m>
class WeightingFunction
{
	using Mbr = typename E::M;


	static constexpr unsigned capacity = E::Node::capacity;
	static constexpr float s = 0.5f;
	static constexpr float shift = exp(-1.0f / (s * s));
	static constexpr float scale = 1.0f / (1 - shift);

	static_assert(2 * m <= capacity, "m must be less than or equal to M/2");

public:

	/**
	 * Construct a new weighting function based on the given entry.
	 *
	 * @param E Parent to base calculations on
	 */
	WeightingFunction(const E& parent) : parent(parent)
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
		dimension = d;

		const Mbr & current = parent.mbr;
		const Mbr original = parent.node->originalMbr();
		const float width = current.getTop()[d] - current.getBottom()[d];

		// Recalculate cached values
		float asym = width == 0.0f ? 0.0f : (
					(current.getTop()[d] + current.getBottom()[d])
					- (original.getTop()[d] + original.getBottom()[d])
				) / width;

		assert(asym <= 1.0f && asym >= -1.0f);

		preShift = 0.5f * (capacity - 1.0f + (capacity + 1.0f - 2.0f * m) * asym);
		preScale = 2.0f / (s * (capacity + 1 + (capacity + 1 - 2 * m) * fabs(asym)));
	}


	/**
	 * Calculate the value of the weighting function.
	 *
	 * @param i Split index
	 */
	float operator()(unsigned i) const
	{
		float e = preScale * (i - preShift);
		float r = scale * (exp(-e * e) - shift);

		assert (r <= 1.0f && r >= -1.0f);

		return r;
	}

private:
	const E& parent;
	Mbr orginial;

	// Used to cache the below result
	unsigned dimension = 0;

	// Temporary results
	float preShift;
	float preScale;


};

};
