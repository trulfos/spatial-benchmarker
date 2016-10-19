#include "SpatialIndex.hpp"

#include "../common/KnnQuery.hpp"
#include "../common/RangeQuery.hpp"
#include <stdexcept>

SpatialIndex::~SpatialIndex()
{
};

ResultSet SpatialIndex::search(const Query& query)
{
	switch (query.getType()) {
		case Query::Type::RANGE:
		{
			const RangeQuery * rq = static_cast<const RangeQuery *>(&query);
			return this->rangeSearch(rq->box);
		}

		case Query::Type::KNN:
		{
			const KnnQuery * kq = static_cast<const KnnQuery *>(&query);
			return this->knnSearch(kq->k, kq->point);
		}
	}

	throw std::runtime_error("Unknown query type");
}
