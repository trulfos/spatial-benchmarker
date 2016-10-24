#include "Point.hpp"
#include <iomanip>

Point::Point(unsigned int dimension)
{
	resize(dimension);
}

Point::Point(std::initializer_list<float> list) : std::vector<float>(list)
{
}

unsigned int Point::getDimension() const
{
	return size();
}


const float * Point::getCoordinates() const
{
	return data();
}


Point Point::operator-(const Point& other) const
{
	unsigned dimension = getDimension();

	if (dimension != other.getDimension()) {
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
	unsigned dimension = getDimension();

	if (dimension != other.getDimension()) {
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
