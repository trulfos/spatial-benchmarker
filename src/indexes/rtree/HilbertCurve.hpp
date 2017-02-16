#pragma once
#include <array>
#include <cmath>
#include <cassert>
#include <limits>
#include <algorithm>
#include <bitset>
#include "common/Coordinate.hpp"
#include "common/Point.hpp"
#include "common/Box.hpp"

namespace Rtree
{

/**
 * A Hilbert space filling curve with the ability to map between cartesian
 * coordinates and distance along the line (Hilbert value).
 *
 * This is based on the algorithm given in "Calculation of Mappings Between One
 * and n-dimensional Values Using the Hilbert Space-filling Curve" by J. K.
 * Lawder.
 *
 * Most notable differences include
 *  - C++ indexes bitsets from lsb and starts at 0. This makes rotate right more
 *    convenient than rotate left 
 *  - The input is floating point and is converted on-the-fly as needed.
 *
 * @tparam H Data type for hilbert encoded values (unsigned)
 * @tparam D Dimension
 */
template<typename H, unsigned D>
class HilbertCurve
{

public:
	/**
	 * Map a point from cartesian space to Hilbart value.
	 */
	static H map(Point point, const Box& bounds)
	{
		assert(point.getDimension() == D);

		// Normalize all coordinates
		auto points = bounds.getPoints();
		for (unsigned i = 0; i < point.getDimension(); ++i) {
			assert(points.second[i] - points.first[i] > 0.0);
			point[i] = (point[i] - points.first[i]) / (points.second[i] - points.first[i]);
		}

		for (auto c : point) {
			assert(c <= 1.0f && c >= 0.0f);
		}

		// Size of the Hilbert grid (in bits for each dimension)
		constexpr unsigned size = (8 * sizeof(H) + (D - 1)) / D;

		H result = 0;
		unsigned rotation = 0;
		std::bitset<D> mirror;

		// Calculate a chunk of D bits repeatedly till we're done
		for (unsigned i = 0; i < size; ++i) {

			// Extract the i^th bit most significant bit from each coordinate
			std::bitset<D> chunk;

			for (unsigned d = 0; d < D; ++d) {
				point[d] *= 2;

				if (point[d] > 1) {
					point[d] -= 1;
					chunk[D - d - 1] = true;
				}
			}


			// Transform coordinate by mirroring and swapping axes
			chunk = rotateRight(chunk ^ mirror, rotation);

			// Calculate the result for this round
			std::bitset<D> r = leftXor(chunk);

			// Accumulate results
			result <<= std::min(8 * unsigned(sizeof(H)) - i * D, D);
			result |= r.to_ullong();

			// Update transform for next round
			unsigned p = principalPosition(r);

			if (chunk.count() % 2 == 0) {
				chunk[p].flip();
			}

			mirror ^= rotateRight(chunk ^ std::bitset<D>(1), rotation);
			rotation = (rotation + p + 1) % D;
		}

		return result;
	}


private:

	/**
	 * Bitwise left rotation of bitset (circular left shift).
	 *
	 * @param bs Original bitset
	 * @param rotation Number of positions to rotate
	 * @return New rotated version of bs
	 */
	static std::bitset<D> rotateRight(const std::bitset<D>& bs, unsigned rotation)
	{
		assert(rotation < bs.size());
		return (bs >> rotation) | (bs << (bs.size() - rotation));
	}

	/**
	 * Given a bitset Calculates the bitset where the i^th bit, b[i], is given by
	 *     b[i] = a[i] ^ a[i-1] ^ ... ^ b[0]
	 *
	 * (Note that std::bitset counts in the opposite direction)
	 *
	 * @param bs Original bitset a[0],a[1],..,a[n]
	 * @return New bitset b[0],b[1],..,b[n]
	 */
	static std::bitset<D> leftXor(const std::bitset<D>& bs)
	{
		std::bitset<D> result = bs;

		for (unsigned i = 1; i < bs.size(); i++) {
			result ^= (bs >> i);
		}

		return result;
	}

	/**
	 * This calculates the first bit change in the bitset counted from the least
	 * significant bit.
	 *
	 * @param bs Bitset to count in
	 * @return Position of first bit not equal to the last (or 0 if all are
	 *     equal)
	 */
	static unsigned principalPosition(const std::bitset<D>& bs)
	{
		unsigned i = 1;

		while (i < bs.size() && bs[i] == bs[0]) {
			i++;
		}

		return (i % bs.size());
	}

};

};
