#include "RangeQuery.hpp"

RangeQuery::RangeQuery(const AxisAlignedBox& box)
	: Query(Query::Type::RANGE), box(box)
{
}

RangeQuery::RangeQuery(const Point& a, const Point& b)
	: RangeQuery(AxisAlignedBox(a, b))
{
}

std::ostream& operator<<(std::ostream& stream, const RangeQuery& query)
{
	auto& points = query.box.getPoints();
	return stream << "range " << points.first << ' ' << points.second;
}
