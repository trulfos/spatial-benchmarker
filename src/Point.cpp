#include "Point.hpp"
#include <iomanip>

Point::Point(unsigned int dimension) : dimension(dimension)
{
	this->coordinates = std::shared_ptr<float>(
			new float[dimension], std::default_delete<float[]>()
		);
}

unsigned int Point::getDimension() const
{
	return dimension;
}


const float * Point::getCoordinates() const
{
	return coordinates.get();
}


std::istream& operator>>(std::istream& stream, Point& point)
{
	float * coordinates = point.coordinates.get();
	for (unsigned int i = 0; i < point.dimension; i++) {
		stream >> coordinates[i];
	}

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Point& point)
{
	stream << "( " << std::fixed << std::setprecision(2);
	const float * coordinates = point.coordinates.get();
	for (unsigned int i = 0; i < point.dimension; i++) {
		stream << coordinates[i] << ' ';
	}

	stream << ')';

	return stream;
};
