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


void SpatialIndex::search(Results& results, const Query& query) const
{
	switch (query.getType()) {
		case Query::Type::RANGE:
		{
			const RangeQuery * rq = static_cast<const RangeQuery *>(&query);
			rangeSearch(results, rq->getBox());
			return;
		}

		case Query::Type::KNN:
		{
			const KnnQuery * kq = static_cast<const KnnQuery *>(&query);
			knnSearch(results, kq->k, kq->point);
			return;
		}
	}

	throw std::runtime_error("Unknown query type");
}


void SpatialIndex::search(StatsCollector& stats, const Query& query) const
{
	switch (query.getType()) {
		case Query::Type::RANGE:
		{
			const RangeQuery * rq = static_cast<const RangeQuery *>(&query);
			rangeSearch(stats, rq->getBox());
			return;
		}

		case Query::Type::KNN:
		{
			const KnnQuery * kq = static_cast<const KnnQuery *>(&query);
			knnSearch(stats, kq->k, kq->point);
			return;
		}
	}

	throw std::runtime_error("Unknown query type");
}

void SpatialIndex::rangeSearch(StatsCollector& stats, const Box& box) const
{
	throw std::runtime_error("Range search instrumentation not implemented");
}

void SpatialIndex::knnSearch(
		StatsCollector&,
		unsigned k,
		const Point& point
	) const
{
	throw std::runtime_error("k-NN search instrumentation not implemented");
}

}
