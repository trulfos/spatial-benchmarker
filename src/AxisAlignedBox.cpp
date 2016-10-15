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
