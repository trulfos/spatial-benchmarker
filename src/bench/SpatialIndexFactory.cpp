#include "SpatialIndexFactory.hpp"

#include "bench/indexes/naive/SpatialIndex.hpp"
#include "bench/indexes/parallel/SpatialIndex.hpp"
#include "bench/indexes/rtree/SpatialIndex.hpp"
#include "bench/indexes/rtree/QuadraticInsertStrategy.hpp"
#include "bench/indexes/rtree/RStarInsertStrategy.hpp"
#include "bench/indexes/rtree/RRStarInsertStrategy.hpp"
#include "bench/indexes/rtree/GreeneInsertStrategy.hpp"
#include "bench/indexes/sequential/SpatialIndex.hpp"
#include "bench/indexes/vectorized/SpatialIndex.hpp"


std::shared_ptr<SpatialIndex> SpatialIndexFactory::create(
		const std::string& algorithm,
		LazyDataSet& dataSet
) {
	if (algorithm == "naive") {
		return std::make_shared<Naive::SpatialIndex>(dataSet);
	} else if (algorithm == "parallel") {
		return std::make_shared<Parallel::SpatialIndex>(dataSet);
	} else if (algorithm == "sequential") {
		return std::make_shared<Sequential::SpatialIndex>(dataSet);
	} else if (algorithm == "vectorized") {
		return std::make_shared<Vectorized::SpatialIndex>(dataSet);
	} else if (algorithm.substr(0, 5) == "rtree") {
		unsigned d = dataSet.dimension();

		std::string type = "guttman";

		if (algorithm.size() > 5) {
			type = algorithm.substr(6, std::string::npos);
		}

		if (type == "star") {
			return createRtree<Rtree::RStarInsertStrategy>(d, dataSet);
		}

		if (type == "rstar") {
			return createRtree<Rtree::RRStarInsertStrategy>(d, dataSet);
		}

		if (type == "guttman") {
			return createRtree<Rtree::QuadraticInsertStrategy>(d, dataSet);
		}
		
		if (type == "greene") {
			return createRtree<Rtree::GreeneInsertStrategy>(d, dataSet);
		}

		throw std::invalid_argument(type + " is not a valid rtree type");
	}

	throw std::invalid_argument(algorithm + " is not a valid algorithm name");
}

std::vector<std::string> SpatialIndexFactory::keys()
{
	return {"naive", "parallel", "rtree", "sequential", "vectorized"};
}

template<class S>
std::shared_ptr<SpatialIndex> SpatialIndexFactory::createRtree(
		unsigned dimension,
		LazyDataSet& dataSet
) {
	//const unsigned M = 128;
	const unsigned M = 16;

	switch (dimension) {
		case 2:
			return std::make_shared<Rtree::SpatialIndex<2, M, S>>(dataSet);
		case 3:
			return std::make_shared<Rtree::SpatialIndex<3, M, S>>(dataSet);
		case 4:
			return std::make_shared<Rtree::SpatialIndex<4, M, S>>(dataSet);
		case 5:
			return std::make_shared<Rtree::SpatialIndex<5, M, S>>(dataSet);
		case 6:
			return std::make_shared<Rtree::SpatialIndex<6, M, S>>(dataSet);
		case 7:
			return std::make_shared<Rtree::SpatialIndex<7, M, S>>(dataSet);
		case 8:
			return std::make_shared<Rtree::SpatialIndex<8, M, S>>(dataSet);
		case 9:
			return std::make_shared<Rtree::SpatialIndex<9, M, S>>(dataSet);
		case 10:
			return std::make_shared<Rtree::SpatialIndex<10, M, S>>(dataSet);

		default:
			throw std::domain_error(
					"R-tree not compiled for dimension "
						+ std::to_string(dimension)
				);
	}
}
