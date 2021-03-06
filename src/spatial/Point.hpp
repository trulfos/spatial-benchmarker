#pragma once
#include "Coordinate.hpp"
#include <istream>
#include <memory>
#include <ostream>
#include <vector>

namespace Spatial
{

/**
 * Simple class representing a point in R^d where d is the dimension of the
 * point.
 */
class Point : private std::vector<Coordinate>
{
	public:

		using std::vector<Coordinate>::vector;

		Point() = default;

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
		 * Calculates the product of this point with itself.
		 */
		float squared() const
		{
			return (*this) * (*this);
		}

		/**
		 * Get the underlying array.
		 */
		using std::vector<Coordinate>::data;


		/**
		 * Get a specific coordinate of this point.
		 */
		using std::vector<Coordinate>::operator[];

		/**
		 * Compare works as for vector.
		 */
		bool operator==(const Point& other) const;


		// Allow iteration
		using std::vector<Coordinate>::begin;
		using std::vector<Coordinate>::end;

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

}
