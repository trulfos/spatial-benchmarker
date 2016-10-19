#include "KnnQuery.hpp"

KnnQuery::KnnQuery(unsigned k, const Point& point)
	: Query(Query::Type::KNN), k(k), point(point)
{
}
