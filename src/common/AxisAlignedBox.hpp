#pragma once

#include "Point.hpp"
#include <utility>

/**
 * Axis aligned rectangle, defined by two points in space.
 */
class AxisAlignedBox
{
	public:

		/**
		 * Construct an axis aligned box using the two points given.
		 */
		AxisAlignedBox(const Point& a, const Point& b);

		/**
		 * Construct an axis aligned box from a point and a width/height.
		 *
		 * @param center Center of box
		 * @param size Size of this box in each direction
		 */
		AxisAlignedBox(const Point& center, float size);


		/**
		 * Get an array of the two points defining this box.
		 */
		const std::pair<const Point&, const Point&> getPoints() const;


		/**
		 * Get dimension of this box.
		 */
		unsigned getDimension() const;


		/**
		 * Check if this axis aligned box contains the given point.
		 */
		bool contains(const Point& p) const;


	private:
		Point points[2];
};
