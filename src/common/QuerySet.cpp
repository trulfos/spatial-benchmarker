#include "QuerySet.hpp"
#include "KnnQuery.hpp"
#include "RangeQuery.hpp"
#include "FileHeader.hpp"

QuerySet::~QuerySet()
{
	for (Query * query : static_cast<std::vector<Query *>>(*this)) {
		delete query;
	}
}


const Query& QuerySet::operator[](size_t i) const
{
	return *std::vector<Query *>::operator[](i);
}


unsigned QuerySet::getDimension() const
{
	if (empty()) {
		throw std::logic_error("Cannot infer dimension of empty query set");
	}

	const Query& query = operator[](0);
	if (query.getType() == Query::Type::KNN) {
		return static_cast<const KnnQuery&>(query)
			.point
			.getDimension();
	}

	return static_cast<const RangeQuery&>(query)
		.box
		.getPoints()
		.first
		.getDimension();
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
			querySet.add(RangeQuery(Box(p1, p2)));
		} else {
			stream.setstate(std::ios_base::failbit);
			return stream;
		}
	}

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const QuerySet& querySet)
{
	stream << FileHeader(
			querySet.getDimension(),
			querySet.size()
		) << '\n';

	for (const Query& query : querySet) {
		switch (query.getType()) {
			case Query::Type::KNN:
				stream << static_cast<const KnnQuery&>(query);
				break;

			case Query::Type::RANGE:
				stream << static_cast<const RangeQuery&>(query);
				break;
		}

		stream << '\n';
	}

	return stream;
}
