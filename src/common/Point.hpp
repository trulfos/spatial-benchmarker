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
		 * Initialize a point with random distribution.
		 */
		template<class D, class G>
		Point(unsigned dimension, D& distribution, G& generator)
			: Point(dimension)
		{
			for (auto& c : *this) {
				c = distribution(generator);
			}
		};

		/**
		 * Get the dimension of this point.
		 */
		unsigned getDimension() const;


		/**
		 * Write this point to a stream in binary.
		 */
		void write(std::ostream& stream) const;

		/**
		 * Read this point from a stream in binary.
		 */
		void read(std::istream& stream);


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
		using std::vector<Coordinate>::operator[];

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
