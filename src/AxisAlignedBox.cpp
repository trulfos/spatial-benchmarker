#include "AxisAlignedBox.hpp"

AxisAlignedBox::AxisAlignedBox(const Point& a, const Point& b)
	: points({a, b})
{
	unsigned dimension = a.getDimension();

	// Check point dimensions match
	if (dimension != b.getDimension()) {
		throw new std::logic_error(
				"Dimensions of points for axis aligned box do not match"
			);
	}

	// Check point domination
	for (unsigned i = 0; i < dimension; i++) {
		if (a.getCoordinates()[i] > b.getCoordinates()[i]) {
			throw new std::logic_error(
					"Point a passed to axis aligned box does not dominate point 2"
				);
		}
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

	if (this->getDimension() != dimension) {
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
