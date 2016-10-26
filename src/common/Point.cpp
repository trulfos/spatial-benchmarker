#include "Point.hpp"
#include <iomanip>
#include <iterator>

Point::Point(unsigned int dimension)
{
	resize(dimension);
}

Point::Point(std::initializer_list<Coordinate> list) : std::vector<Coordinate>(list)
{
}

unsigned int Point::getDimension() const
{
	return size();
}


Point Point::operator-(const Point& other) const
{
	unsigned dimension = getDimension();

	if (dimension != other.getDimension()) {
		throw std::logic_error(
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
		throw std::logic_error(
				"Cannot multiply points of different dimension"
			);
	}

	float result = 0.0f;

	for (unsigned i = 0; i < dimension; i++) {
		result += (*this)[i] * other[i];
	}

	return result;
}


bool Point::operator==(const Point& other) const
{
	// Delegate to vector
	return static_cast<const std::vector<Coordinate>&>(*this) == other;
}

std::istream& operator>>(std::istream& stream, Point& point)
{
	for (Coordinate& c : point) {
		stream >> c;
	}

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Point& point)
{
	stream << std::fixed << std::setprecision(2);

	std::copy(
			point.begin(),
			point.end(),
			std::ostream_iterator<Coordinate>(stream, " ")
		);

	return stream;
};
