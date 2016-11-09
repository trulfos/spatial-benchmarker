#include "SpatialIndexFactory.hpp"

#include "bench/indexes/naive/SpatialIndex.hpp"
#include "bench/indexes/parallel/SpatialIndex.hpp"
#include "bench/indexes/rtree/SpatialIndex.hpp"
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
	} else if (algorithm == "rtree") {
		const unsigned M = 128;
		unsigned d = dataSet.dimension();
		switch (d) {
			case 2:
				return std::make_shared<Rtree::SpatialIndex<2, M>>(dataSet);
			case 3:
				return std::make_shared<Rtree::SpatialIndex<3, M>>(dataSet);
			case 4:
				return std::make_shared<Rtree::SpatialIndex<4, M>>(dataSet);
			case 5:
				return std::make_shared<Rtree::SpatialIndex<5, M>>(dataSet);
			case 6:
				return std::make_shared<Rtree::SpatialIndex<6, M>>(dataSet);
			case 7:
				return std::make_shared<Rtree::SpatialIndex<7, M>>(dataSet);
			case 8:
				return std::make_shared<Rtree::SpatialIndex<8, M>>(dataSet);
			case 9:
				return std::make_shared<Rtree::SpatialIndex<9, M>>(dataSet);
			case 10:
				return std::make_shared<Rtree::SpatialIndex<10, M>>(dataSet);

			default:
				throw std::domain_error(
						"R-tree not compiled for dimension " + std::to_string(d)
					);
		}
	}

	throw std::invalid_argument(algorithm + " is not a valid algorithm name");
}

std::vector<std::string> SpatialIndexFactory::keys()
{
	return {"naive", "parallel", "rtree", "sequential", "vectorized"};
}
