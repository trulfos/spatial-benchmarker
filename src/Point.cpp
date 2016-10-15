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


Point Point::operator-(const Point& other) const
{
	if (dimension != other.dimension) {
		throw new std::logic_error(
				"Cannot subtract points of different dimension"
			);
	}

	Point result (dimension);

	for (unsigned i = 0; i < dimension; i++) {
		result[i] = (*this)[i] - other[i];
	}

	return result;
}


float Point::operator*(const Point& other) const
{
	if (dimension != other.dimension) {
		throw new std::logic_error(
				"Cannot multiply points of different dimension"
			);
	}

	float result = 0.0f;

	for (unsigned i = 0; i < dimension; i++) {
		result += (*this)[i] * other[i];
	}

	return result;
}


float& Point::operator[](unsigned i)
{
	return std::vector<float>::operator[](i);
}

const float& Point::operator[](unsigned i) const
{
	return std::vector<float>::operator[](i);
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
