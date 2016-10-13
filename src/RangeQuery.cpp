#include "RangeQuery.hpp"

RangeQuery::RangeQuery(const AxisAlignedBox& box)
	: Query(Query::Type::RANGE), box(box)
{
}
