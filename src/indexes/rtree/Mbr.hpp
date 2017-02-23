#pragma once
#include <cassert>
#include "common/Box.hpp"
#include "common/Coordinate.hpp"
#include "common/Point.hpp"

namespace Rtree
{

/**
 * Minimum bounding rectangle used in the R-tree algorithm.
 */
template<unsigned D>
class Mbr
{
	public:
		Mbr()
		{
		};

		/**
		 * Create MBR from the given point.
		 * The MBR will have no volume.
		 */
		Mbr(const Point& p)
		{
			if (p.getDimension() != D) {
				throw std::logic_error(
						"Point of wrong dimension given to MBR"
					);
			}

			for (unsigned i = 0; i < D; i++) {
				top[i] = bottom[i] = p[i];
			}
		};

		/**
		 * Create MBR from axis aligned box.
		 */
		Mbr(const Box& box)
		{
			if (box.getDimension() != D) {
				throw std::logic_error(
						"Axis aligned box of wrong dimension given to MBR"
					);
			}

			auto points = box.getPoints();

			for (unsigned i = 0; i < D; i++) {
				bottom[i] = points.first[i];
				top[i] = points.second[i];
			}
		};

		/**
		 * Adding two MBRs gives the MBR containing both.
		 *
		 * @param other Another MBR
		 * @return The MBR containing both this and other
		 */
		Mbr operator+(const Mbr& other) const
		{
			Mbr result;

			for (unsigned i = 0; i < D; i++) {
				result.top[i] = std::max(other.top[i], top[i]);
				result.bottom[i] = std::min(other.bottom[i], bottom[i]);
			}

			return result;
		};

		Mbr& operator+=(const Mbr& other)
		{
			return *this = this->operator+(other);
		}

		/**
		 * Calculate the hypervolume.
		 *
		 * @return Hypervolume of this.
		 */
		double volume() const
		{
			double v = 1.0f;
			for (unsigned i = 0; i < D; i++) {
				v *= top[i] - bottom[i];
			}
			return v;
		};

		/**
		 * Calculate volume enlargement necessary to add the given MBR.
		 *
		 * @param Other MBR to include
		 * @return Necessary enlargement
		 */
		double enlargement(const Mbr& other) const
		{
			return (*this + other).volume() - volume();
		};

		/**
		 * Check whether this Mbr intersects another.
		 *
		 * @param other Other MBR to intersect
		 * @return True if intersects
		 */
		bool intersects(const Mbr& other) const
		{
			for (unsigned i = 0; i < D; i++) {
				if (top[i] < other.bottom[i] || other.top[i] < bottom[i]) {
					return false;
				}
			}

			return true;
		};


		/**
		 * Calculate the MBR of the intersection between this and another MBR.
		 *
		 * @param other Other MBR
		 * @return MBR of intersection
		 */
		Mbr intersection(const Mbr& other) const
		{
			Mbr result;

			assert(intersects(other));

			for (unsigned i = 0; i < D; i++) {
				result.top[i] = std::min(other.top[i], top[i]);
				result.bottom[i] = std::max(other.bottom[i], bottom[i]);
			}

			return result;
		};


		/**
		 * Check whether this MBR contains another.
		 *
		 * @param other Other MBR
		 * @return True if other is contained in this.
		 */
		bool contains(const Mbr& other) const
		{
			for (unsigned i = 0; i < D; i++) {
				if (bottom[i] > other.bottom[i] || top[i] < other.top[i]) {
					return false;
				}
			}

			return true;
		};


		/**
		 * Calculate the distance between two MBRs to the power of 2.
		 *
		 * @param other Other MBR
		 * @return Distance between this and other
		 */
		double distance2(const Mbr& other) const
		{
			double d = 0.0f;

			for (unsigned i = 0; i < D; i++) {
				double diff = distanceAlong(i, other);

				d += diff * diff;
			}

			return d;
		};


		/**
		 * Calculate the distance between two MBRs along a single dimension.
		 *
		 * @param d Dimension
		 * @param other Other MBR
		 *
		 * @return Distance between other and this along d
		 */
		double distanceAlong(unsigned d, const Mbr& other) const
		{
			return std::max(
					std::max(
						0.0,
						(double) (bottom[d] - other.top[d])
					),
					(double) (other.bottom[d] - top[d])
				);
		};


		/**
		 * Calculate the perimeter of this MBR.
		 *
		 * Note that this is the perimeter as defined by Beckmann and Seeger,
		 * which is simply the sum of the length in each dimension. This
		 * generalizes easier to more dimensions and avoids multiplying by a
		 * useless constant.
		 *
		 * @return Perimeter
		 */
		double perimeter() const
		{
			double perimeter = 0.0f;

			for (unsigned d = 0; d < D; ++d) {
				perimeter += top[d] - bottom[d];
			}

			return perimeter;
		};


		/**
		 * Calculates the center of this MBR.
		 *
		 * @return Point representing at the location of the center
		 */
		Point center() const
		{
			Point result (D);
			for (unsigned d = 0; d < D; ++d) {
				result[d] = (top[d] + bottom[d]) / 2.0f;
			}

			return result;
		}


		/**
		 * Get bottom coordinates.
		 */
		const Coordinate * getBottom() const
		{
			return bottom;
		};


		/**
		 * Get top coordinates.
		 */
		const Coordinate * getTop() const
		{
			return top;
		};


		/**
		 * Calculate the enlargement of the overlap between this and another MBR
		 * if a new MBR is added to this.
		 *
		 * @param original Original MBR of the first subject
		 * @param other Other MBR to compute overlap with
		 * @param n MBR to add
		 * @param measure Function to evaluate overlap
		 */
		template<class F>
		double overlapEnlargement(const Mbr& other, const Mbr& n, F measure)
		{
			Mbr enlarged = *this + n;

			// They may not overlap at all
			if (!enlarged.intersects(other)) {
				return 0.0;
			}


			double overlap = measure(enlarged.intersection(other));

			if (intersects(other)) {
				overlap -= measure(intersection(other));
			}

			return overlap;
		}

	private:
		Coordinate top[D];
		Coordinate bottom[D];
};

}
