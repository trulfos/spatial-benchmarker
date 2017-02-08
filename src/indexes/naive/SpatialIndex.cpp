#include "SpatialIndex.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath>

namespace Naive
{

/**
 * Calculates the distance between two points.
 */
float d(const Point& a, const Point& b) {
	Point diff = a - b;
	return sqrt(diff * diff);
}


void SpatialIndex::load(LazyDataSet& data)
{
	dataSet.assign(data.begin(), data.end());
};


Results SpatialIndex::rangeSearch(const Box& box) const
{
	std::vector<DataObject> results;

	// Filter items
	std::copy_if(
			dataSet.begin(),
			dataSet.end(),
			std::back_inserter(results),
			[&](const DataObject& object) -> bool {
				return box.intersects(object.getBox());
			}
		);

	// Extract ids
	Results resultSet (results.size());
	std::transform(
			results.begin(),
			results.end(),
			resultSet.begin(),
			[](const DataObject& object) {
				return object.getId();
			}
		);

	return resultSet;
};

Results SpatialIndex::knnSearch(unsigned k, const Point& point) const
{
	throw std::logic_error("KNN search not implemented");
};

}
