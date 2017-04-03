#pragma once
#include "Box.hpp"
#include "Query.hpp"

namespace Spatial
{

class RangeQuery : public Query
{
	public:
		RangeQuery() = default;
		RangeQuery(unsigned id, const Box& box);
		RangeQuery(unsigned id, const Point& a, const Point& b);

		std::string getName() const;
		const Box& getBox() const;

	private:
		Box box;
		unsigned id;

};

std::ostream& operator<<(std::ostream& stream, const RangeQuery& query);

}
