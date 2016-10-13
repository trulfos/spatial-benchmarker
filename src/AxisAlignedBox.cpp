#include "AxisAlignedBox.hpp"

AxisAlignedBox::AxisAlignedBox(const Point& a, const Point& b)
	: points({a, b})
{
	//TODO: Check dimension match
	//TODO: Check that a < b for all coordinates
}

const std::pair<const Point&, const Point&> AxisAlignedBox::getPoints() const
{
	return std::pair<const Point&, const Point&>(points[0], points[1]);
}
