#include "KnnQuery.hpp"

KnnQuery::KnnQuery(unsigned k, const Point& point)
	: Query(Query::Type::KNN), k(k), point(point)
{
}

std::ostream& operator<<(std::ostream& stream, const KnnQuery& query)
{
	return stream << "knn " << query.k << ' ' << query.point;
}
