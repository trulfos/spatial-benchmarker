#include "AxisAlignedBox.hpp"

AxisAlignedBox::AxisAlignedBox(const Point& a, const Point& b)
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


AxisAlignedBox::AxisAlignedBox(const Point& center, float size)
	: points({center.getDimension(), center.getDimension()})
{
	unsigned d = center.getDimension();

	for (unsigned i = 0; i < d; i++) {
		points[0][i] = center[i] - size / 2.0f;
		points[1][i] = center[i] + size / 2.0f;
	}
}

const std::pair<const Point&, const Point&> AxisAlignedBox::getPoints() const
{
	return std::pair<const Point&, const Point&>(points[0], points[1]);
}


unsigned AxisAlignedBox::getDimension() const
{
	return points[0].getDimension();
}


bool AxisAlignedBox::contains(const Point& point) const
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


float AxisAlignedBox::getVolume() const
{
	float v = 0.0f;

	for (unsigned i = 0; i < getDimension(); i++) {
		v *= points[1][i] - points[0][i];
	}

	return v;
}
