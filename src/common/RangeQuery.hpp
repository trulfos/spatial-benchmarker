#pragma once

#include "AxisAlignedBox.hpp"
#include "Query.hpp"

class RangeQuery : public Query
{
	public:
		const AxisAlignedBox box;

		RangeQuery(const AxisAlignedBox& box);
		RangeQuery(const Point& a, const Point& b);

};

std::ostream& operator<<(std::ostream& stream, const RangeQuery& query);
