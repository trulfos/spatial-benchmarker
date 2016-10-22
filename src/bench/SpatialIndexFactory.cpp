#include "SpatialIndexFactory.hpp"

#include "NaiveSpatialIndex.hpp"
#include "ParallelSpatialIndex.hpp"
#include "rtree/SpatialIndex.hpp"


SpatialIndexFactory::SpatialIndexFactory(
		const std::vector<DataObject>& dataSet
) : dataSet(dataSet) {
}

std::shared_ptr<SpatialIndex> SpatialIndexFactory::create(
		const std::string& algorithm
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

		unsigned d = dataSet[0].getPoint().getDimension();
		switch (d) {
			case 2:
				return std::make_shared<Rtree::SpatialIndex<2, 3>>(dataSet);
			case 3:
				return std::make_shared<Rtree::SpatialIndex<3, 3>>(dataSet);
			case 4:
				return std::make_shared<Rtree::SpatialIndex<4, 3>>(dataSet);
			case 5:
				return std::make_shared<Rtree::SpatialIndex<5, 3>>(dataSet);
			case 6:
				return std::make_shared<Rtree::SpatialIndex<6, 3>>(dataSet);
			case 7:
				return std::make_shared<Rtree::SpatialIndex<7, 3>>(dataSet);
			case 8:
				return std::make_shared<Rtree::SpatialIndex<8, 3>>(dataSet);
			case 9:
				return std::make_shared<Rtree::SpatialIndex<9, 3>>(dataSet);

			default:
				throw std::domain_error(
						"R-tree not compiled for dimension " + d
					);
		}
	}

	throw std::invalid_argument(algorithm + " is not a valid algorithm name");
}
