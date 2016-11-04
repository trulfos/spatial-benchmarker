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


SpatialIndex::SpatialIndex(LazyDataSet& dataSet)
	: dataSet(dataSet.begin(), dataSet.end())
{
};

SpatialIndex::SpatialIndex()
{
};


Results SpatialIndex::rangeSearch(const AxisAlignedBox& box) const
{
	std::vector<DataObject> results;

	// Filter items
	std::copy_if(
			dataSet.begin(),
			dataSet.end(),
			std::back_inserter(results),
			[&](const DataObject& object) -> bool {
				return box.contains(object.getPoint());
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
	unsigned resultSize = std::min(k, (unsigned) dataSet.size());

	std::vector<DataObject> dataResults (
			dataSet.begin(),
			dataSet.begin() + resultSize
		);

	// Sort it by distance, then by id
	std::partial_sort_copy(
			dataSet.begin(),
			dataSet.end(),
			dataResults.begin(),
			dataResults.end(),
			[&point](const DataObject& a, const DataObject& b) -> bool {
				float da = d(point, a.getPoint());
				float db = d(point, b.getPoint());
				return da < db || (da == db && a.getId() < b.getId());
			}
		);


	// Generate results
	Results results (resultSize);

	std::transform(
			dataResults.begin(),
			dataResults.begin() + resultSize,
			results.begin(),
			[](const DataObject& object) -> DataObject::Id {
				return object.getId();
			}
		);

	return results;
};

}
