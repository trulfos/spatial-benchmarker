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
		 * Get an array of the two points defining this box.
		 */
		const std::pair<const Point&, const Point&> getPoints() const;


	private:
		Point points[2];
};
