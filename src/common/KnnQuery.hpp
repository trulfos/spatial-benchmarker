#pragma once

#include "Query.hpp"

class KnnQuery : public Query
{
	public:
		const unsigned k;
		const Point point;

		KnnQuery(unsigned k, const Point& point);
};
