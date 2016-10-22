#pragma once
#include <istream>
#include <memory>
#include <ostream>
#include <vector>

/**
 * Simple class representing a point in R^d where d is the dimension of the
 * point.
 */
class Point : private std::vector<float>
{
	public:

		/**
		 * Create a point of the given dimension.
		 */
		Point(unsigned dimension);

		/**
		 * Get the dimension of this point.
		 */
		unsigned getDimension() const;

		/**
		 * Get a pointer to the list of coordinates.
		 *
		 * Note that this list may contain garbage if this point has not yet
		 * been initialized.
		 */
		const float * getCoordinates() const;


		/**
		 * Calculate a new point - the difference between two other points.
		 */
		Point operator-(const Point& other) const;

		/**
		 * Calulates the scalar product of this point and another.
		 */
		float operator*(const Point& other) const;


		/**
		 * Get a specific coordinate of this point.
		 */
		float& operator[](unsigned i);
		const float& operator[](unsigned i) const;


	private:
		unsigned dimension;

		/**
		 * Custom read from istreams.
		 */
		friend std::istream& operator>>(std::istream&, Point&);

		/**
		 * Custom print to ostreams
		 */
		friend std::ostream& operator<<(std::ostream&, const Point&);
};
