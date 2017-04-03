#pragma once

#include "Point.hpp"
#include <utility>

namespace Spatial {

/**
 * Axis aligned rectangle, defined by two points in space.
 */
class Box
{
	public:

		/**
		 * Construct an axis aligned box using the two points given.
		 */
		Box(const Point& a, const Point& b);

		Box() = default;

		/**
		 * Construct empty box with the given dimension.
		 */
		Box(unsigned dimension);

		/**
		 * Construct an axis aligned box from a point and a width/height.
		 *
		 * @param center Center of box
		 * @param size Size of this box in each direction
		 */
		Box(const Point& center, float size);


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


		/**
		 * Check if this axis aligned box intersects another box.
		 */
		bool intersects(const Box& o) const;


		/**
		 * Include the other box in this. The resulting box is the bounding box
		 * of both boxes.
		 */
		void include(const Box& other);


	private:
		Point points[2];
};

}
