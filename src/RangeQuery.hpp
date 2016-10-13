#pragma once

#include "AxisAlignedBox.hpp"
#include "Query.hpp"

class RangeQuery : public Query
{
	public:
		const AxisAlignedBox box;

		RangeQuery(const AxisAlignedBox& box);
};
