#pragma once
#include <memory>
#include <istream>
#include <ostream>

/**
 * Simple class representing a point in R^d where d is the dimension of the
 * point.
 *
 * TODO: It's not clear why points work like pointers. Perhaps copy the array on
 * copy instead of sharing it? Probably not going to copy points all that often
 * anyway.
 */
class Point
{
	public:

		/**
		 * Create a point of the given dimension.
		 */
		Point(unsigned int dimension);

		/**
		 * Get the dimension of this point.
		 */
		unsigned int getDimension() const;

		/**
		 * Get a pointer to the list of coordinates.
		 *
		 * Note that this list may contain garbage if this point has not yet
		 * been initialized.
		 */
		const float * getCoordinates() const;

	private:
		std::shared_ptr<float> coordinates;
		unsigned int dimension;

		/**
		 * Custom read from istreams.
		 */
		friend std::istream& operator>>(std::istream&, Point&);

		/**
		 * Custom print to ostreams
		 */
		friend std::ostream& operator<<(std::ostream&, const Point&);
};
