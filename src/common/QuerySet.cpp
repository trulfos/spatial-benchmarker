#include "QuerySet.hpp"
#include "KnnQuery.hpp"
#include "RangeQuery.hpp"

QuerySet::~QuerySet()
{
	for (Query * query : *this) {
		delete query;
	}
}


Query& QuerySet::operator[](size_t i)
{
	return *std::vector<Query *>::operator[](i);
}

std::istream& operator>>(std::istream& stream, QuerySet& querySet) {

	unsigned dimension;
	unsigned nQueries;

	stream >> dimension >> nQueries;
	
	for (unsigned i = 0; i < nQueries; ++i) {
		std::string type;
		stream >> type;

		if (type == "knn") {
			unsigned k;
			Point p (dimension);
			stream >> k >> p;
			querySet.add(KnnQuery(k, p));
		} else if (type == "range") {
			Point p1 (dimension), p2 (dimension);
			stream >> p1 >> p2;
			querySet.add(RangeQuery(AxisAlignedBox(p1, p2)));
		} else {
			stream.setstate(std::ios_base::failbit);
			return stream;
		}
	}

	return stream;
}
