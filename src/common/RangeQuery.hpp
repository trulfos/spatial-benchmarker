#pragma once

#include "Box.hpp"
#include "Query.hpp"

class RangeQuery : public Query
{
	public:
		const Box box;

		RangeQuery(const Box& box);
		RangeQuery(const Point& a, const Point& b);

		std::string getName() const;

};

std::ostream& operator<<(std::ostream& stream, const RangeQuery& query);
