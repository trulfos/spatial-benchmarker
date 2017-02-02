#pragma once

#include "Box.hpp"
#include "Query.hpp"

class RangeQuery : public Query
{
	public:
		RangeQuery() = default;
		RangeQuery(const Box& box);
		RangeQuery(const Point& a, const Point& b);

		std::string getName() const;
		const Box& getBox() const;

	private:
		Box box;

};

std::ostream& operator<<(std::ostream& stream, const RangeQuery& query);
