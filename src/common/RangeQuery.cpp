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

const Box& RangeQuery::getBox() const
{
	return box;
}

std::ostream& operator<<(std::ostream& stream, const RangeQuery& query)
{
	auto& points = query.getBox().getPoints();
	return stream << "range " << points.first << ' ' << points.second;
}
