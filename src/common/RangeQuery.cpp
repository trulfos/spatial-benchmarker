#include "RangeQuery.hpp"

RangeQuery::RangeQuery(unsigned id, const Box& box)
	: Query(Query::Type::RANGE), box(box), id(id)
{
}

RangeQuery::RangeQuery(unsigned id, const Point& a, const Point& b)
	: RangeQuery(id, Box(a, b))
{
}

std::string RangeQuery::getName() const
{
	return std::to_string(id);
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
