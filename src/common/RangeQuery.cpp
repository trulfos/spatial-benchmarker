#include "RangeQuery.hpp"

RangeQuery::RangeQuery(const Box& box)
	: Query(Query::Type::RANGE), box(box)
{
}

RangeQuery::RangeQuery(const Point& a, const Point& b)
	: RangeQuery(Box(a, b))
{
}

std::string RangeQuery::getName() const
{
	return std::to_string(1.0f);
}

std::ostream& operator<<(std::ostream& stream, const RangeQuery& query)
{
	auto& points = query.box.getPoints();
	return stream << "range " << points.first << ' ' << points.second;
}
