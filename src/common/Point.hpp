#pragma once
#include "Coordinate.hpp"
#include <istream>
#include <memory>
#include <ostream>
#include <vector>

/**
 * Simple class representing a point in R^d where d is the dimension of the
 * point.
 */
class Point : private std::vector<Coordinate>
{
	public:

		/**
		 * Create a point of the given dimension.
		 */
		Point(unsigned dimension);

		/**
		 * Create a point from a list of coordinates.
		 */
		Point(std::initializer_list<Coordinate> list);

		/**
		 * Get the dimension of this point.
		 */
		unsigned getDimension() const;


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
		Coordinate& operator[](unsigned i);
		const Coordinate& operator[](unsigned i) const;

		/**
		 * Compare works as for vector.
		 */
		bool operator==(const Point& other) const;

	private:
		/**
		 * Custom read from istreams.
		 */
		friend std::istream& operator>>(std::istream&, Point&);

		/**
		 * Custom print to ostreams
		 */
		friend std::ostream& operator<<(std::ostream&, const Point&);
};
