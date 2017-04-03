#include "SpatialIndex.hpp"
#include "KnnQuery.hpp"
#include "RangeQuery.hpp"
#include <stdexcept>

namespace Spatial
{

SpatialIndex::~SpatialIndex()
{
}

void SpatialIndex::checkStructure() const
{
}


StatsCollector SpatialIndex::collectStatistics() const
{
	throw std::runtime_error("This index does not support stats collection");
}


void SpatialIndex::prepare()
{
};


Results SpatialIndex::search(const Query& query) const
{
	switch (query.getType()) {
		case Query::Type::RANGE:
		{
			const RangeQuery * rq = static_cast<const RangeQuery *>(&query);
			return rangeSearch(rq->getBox());
		}

		case Query::Type::KNN:
		{
			const KnnQuery * kq = static_cast<const KnnQuery *>(&query);
			return knnSearch(kq->k, kq->point);
		}
	}

	throw std::runtime_error("Unknown query type");
}


Results SpatialIndex::search(const Query& query, StatsCollector& stats) const
{
	switch (query.getType()) {
		case Query::Type::RANGE:
		{
			const RangeQuery * rq = static_cast<const RangeQuery *>(&query);
			return rangeSearch(rq->getBox(), stats);
		}

		case Query::Type::KNN:
		{
			const KnnQuery * kq = static_cast<const KnnQuery *>(&query);
			return knnSearch(kq->k, kq->point, stats);
		}
	}

	throw std::runtime_error("Unknown query type");
}

Results SpatialIndex::rangeSearch(const Box& box, StatsCollector&) const
{
	throw std::runtime_error("Range search instrumentation not implemented");
}

Results SpatialIndex::knnSearch(
		unsigned k,
		const Point& point,
		StatsCollector&
	) const
{
	throw std::runtime_error("k-NN search instrumentation not implemented");
}

}
