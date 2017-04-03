#pragma once
#include "Query.hpp"

namespace Spatial
{

class KnnQuery : public Query
{
	public:
		const unsigned k;
		const Point point;

		KnnQuery(unsigned k, const Point& point);

		std::string getName() const;
};

std::ostream& operator<<(std::ostream& stream, const KnnQuery& query);

}
