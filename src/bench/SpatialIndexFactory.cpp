#include "SpatialIndexFactory.hpp"

#include "NaiveSpatialIndex.hpp"
#include "ParallelSpatialIndex.hpp"
#include "rtree/SpatialIndex.hpp"


std::shared_ptr<SpatialIndex> SpatialIndexFactory::create(
		const std::string& algorithm,
		LazyDataSet& dataSet
) {
	if (algorithm == "naive") {
		return std::make_shared<NaiveSpatialIndex>(dataSet);
	} else if (algorithm == "parallel") {
		return std::make_shared<ParallelSpatialIndex>(dataSet);
	} else if (algorithm == "rtree") {

		//TODO: This can be solved by passing the dimensionality
		if (dataSet.size() < 1) {
			throw std::logic_error("Cannot infer dimension without any points");
		}

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
	return {"naive", "parallel", "rtree"};
}
