#pragma once
#include "../../common/AxisAlignedBox.hpp"
#include "../../common/Coordinate.hpp"
#include "../../common/Point.hpp"

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
		Mbr(const AxisAlignedBox& box)
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
		 * Adding to MBRs gives the MBR containing both.
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
		float volume() const
		{
			float v = 1.0f;
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
		float enlargement(const Mbr& other) const
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
		float distance2(const Mbr& other)
		{
			float d = 0.0f;

			for (unsigned i = 0; i < D; i++) {
				float diff = std::max(
						std::max(0.0f, (float) bottom[i] - other.top[i]),
						(float) other.bottom[i] - top[i]
					);

				d += diff * diff;
			}

			return d;
		};

	private:
		Coordinate top[D];
		Coordinate bottom[D];
};

}
