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


		bool operator!=(const Mbr& other) const
		{
			for (unsigned d = 0; d < D; ++d) {
				if (top[d] != other.top[d] || bottom[d] != other.bottom[d]) {
					return true;
				}
			}

			return false;
		}


		bool operator==(const Mbr& other) const
		{
			return !(*this != other);
		}

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
		 * Calculates the intersection complexity.
		 *
		 * The intersection complexity is the number of dimensions in which this
		 * MBR cuts the other.
		 *
		 * @return Intersection complexity in the range [0, D]
		 */
		unsigned intersectionComplexity(const Mbr& other) const
		{
			unsigned complexity = 0;

			for (unsigned d = 0; d < D; d++) {
				if (top[d] < other.top[d] || bottom[d] > other.bottom[d]) {
					complexity++;
				}
			}

			return complexity;
		}


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
		 * Calculates the delta of some function when including the given MBR.
		 *
		 * This follows the definition by Becmann and Seeger (2009).
		 *
		 * @param measure Function to calculate the delta for
		 * @param other MBR to include
		 * @return The difference in the function value with/without ot other
		 */
		double delta(double (Mbr::*measure)() const, const Mbr& other) const
		{
			return ((*this + other).*measure)() - (this->*measure)();
		}


		/**
		 * Calculates the overlap of this MBR with another.
		 *
		 * @param other Other MBR
		 * @param measure Measurement for overlap
		 * @return Overlap as calculated by measure
		 */
		double overlap(double (Mbr::*measure)() const, const Mbr& other) const
		{
			if (!this->intersects(other)) {
				return 0.0;
			}

			return (this->intersection(other).*measure)();
		}


		/**
		 * Calculate the enlargement of the overlap between this and another MBR
		 * if a new MBR is added to this.
		 *
		 * @param other Other MBR to compute overlap with
		 * @param n MBR to add
		 * @param measure Member function to use for evaluating overlap
		 */
		double deltaOverlap(
				const Mbr& other,
				const Mbr& omega,
				double (Mbr::*measure)() const
			) const
		{
			Mbr enlarged = *this + omega;

			// They may not overlap at all
			if (!enlarged.intersects(other)) {
				return 0.0;
			}


			double overlap = (enlarged.intersection(other).*measure)();

			if (intersects(other)) {
				overlap -= (intersection(other).*measure)();
			}

			return overlap;
		}


		/**
		 * Calculates the wasted volume in the combined MBR if another MBR is
		 * added to this.
		 *
		 * @param other Another MBR to add
		 */
		double waste(const Mbr& other) const
		{
			return (*this + other).volume() - (volume() + other.volume());
		}


	private:
		Coordinate top[D];
		Coordinate bottom[D];
};

}


template<unsigned D>
std::ostream& operator<<(std::ostream& stream, const Rtree::Mbr<D>& mbr)
{
	for (unsigned d = 0; d < D; d++) {
		stream << mbr.getBottom()[d] << " " << mbr.getTop()[d] << '\n';
	}
	return stream;
}
