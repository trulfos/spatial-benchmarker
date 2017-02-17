#include "Box.hpp"

Box::Box(const Point& a, const Point& b)
	: points({a, b})
{

	// Check point dimensions match
	if (a.getDimension() != b.getDimension()) {
		throw std::logic_error(
				"Dimensions of points for axis aligned box do not match"
			);
	}

	unsigned dimension = a.getDimension();

	for (unsigned i = 0; i < dimension; i++) {
		points[0][i] = std::min(a[i], b[i]);
		points[1][i] = std::max(a[i], b[i]);
	}
}


Box::Box(unsigned dimension)
	:points({Point(dimension), Point(dimension)})
{
};


Box::Box(const Point& center, float size)
	: points({Point(center.getDimension()), Point(center.getDimension())})
{
	unsigned d = center.getDimension();

	for (unsigned i = 0; i < d; i++) {
		points[0][i] = center[i] - size / 2.0f;
		points[1][i] = center[i] + size / 2.0f;
	}
}

const std::pair<const Point&, const Point&> Box::getPoints() const
{
	return std::pair<const Point&, const Point&>(points[0], points[1]);
}


unsigned Box::getDimension() const
{
	return points[0].getDimension();
}


bool Box::contains(const Point& point) const
{
	unsigned dimension = point.getDimension();

	if (getDimension() != dimension) {
		throw std::invalid_argument(
			"Cannot check containment for point and box of different dimension"
		);
	}

	bool isWithin = true;
	for (unsigned i = 0; i < dimension; i++) {
		isWithin &= points[0][i] <= point[i] && point[i] <= points[1][i];
	}

	return isWithin;
}


bool Box::intersects(const Box& other) const
{
	unsigned dimension = other.getDimension();

	if (getDimension() != dimension) {
		throw std::invalid_argument(
			"Cannot check intersection for boxes of different dimension"
		);
	}

	for (unsigned i = 0; i < dimension; i++) {
		if (
				points[1][i] < other.points[0][i] ||
				other.points[1][i] < points[0][i]
		) {
			return false;
		}
	}

	return true;
}


float Box::getVolume() const
{
	float v = 0.0f;

	for (unsigned i = 0; i < getDimension(); i++) {
		v *= points[1][i] - points[0][i];
	}

	return v;
}


void Box::include(const Box& other)
{
	for (unsigned i = 0; i < getDimension(); ++i) {
			points[0][i] = std::min(points[0][i], other.points[0][i]);
			points[1][i] = std::max(points[1][i], other.points[1][i]);
	}
}

std::istream& operator>>(
		std::istream& stream,
		Box& box
) {
	//TODO
	throw std::logic_error("Not implemented :-(");
};

std::ostream& operator<<(
		std::ostream& stream,
		const Box& box
) {
	//TODO
	throw std::logic_error("Not implemented :-(");
};
