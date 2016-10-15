#include "Point.hpp"
#include <iomanip>

Point::Point(unsigned int dimension) : dimension(dimension)
{
	this->resize(dimension);
}

unsigned int Point::getDimension() const
{
	return this->size();
}


const float * Point::getCoordinates() const
{
	return &this->operator[](0);
}


std::istream& operator>>(std::istream& stream, Point& point)
{
	for (float& c : point) {
		stream >> c;
	}

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Point& point)
{
	stream << "( " << std::fixed << std::setprecision(2);

	for (const float& c : point) {
		stream << c;
	}

	stream << ')';

	return stream;
};
