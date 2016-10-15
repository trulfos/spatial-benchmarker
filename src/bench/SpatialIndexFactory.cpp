#include "SpatialIndexFactory.hpp"

#include "NaiveSpatialIndex.hpp"
#include "ParallelSpatialIndex.hpp"
#include "RtreeSpatialIndex.hpp"


SpatialIndexFactory::SpatialIndexFactory(
		const std::vector<DataObject>& dataSet
) : dataSet(dataSet) {
}

std::shared_ptr<SpatialIndex> SpatialIndexFactory::create(
		const std::string& algorithm
) {
	if (algorithm == "naive") {
		return std::make_shared<NaiveSpatialIndex>(this->dataSet);
	} else if (algorithm == "parallel") {
		return std::make_shared<ParallelSpatialIndex>(this->dataSet);
	} else if (algorithm == "rtree") {
		return std::make_shared<RtreeSpatialIndex>(this->dataSet);
	}

	throw std::invalid_argument(algorithm + " is not a valid algorithm name");
}
