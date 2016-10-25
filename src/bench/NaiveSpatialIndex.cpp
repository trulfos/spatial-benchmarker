#include "NaiveSpatialIndex.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath>

/**
 * Calculates the distance between two points.
 */
float d(const Point& a, const Point& b) {
	Point diff = a - b;
	return sqrt(diff * diff);
}


NaiveSpatialIndex::NaiveSpatialIndex(const DataSet& dataSet) : dataSet(dataSet)
{
};

NaiveSpatialIndex::~NaiveSpatialIndex()
{
};


Results NaiveSpatialIndex::rangeSearch(const AxisAlignedBox& box) const
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

Results NaiveSpatialIndex::knnSearch(unsigned k, const Point& point) const
{
	// Copy the data set
	DataSet sortedDataSet = dataSet;

	// Sort it by distance, then by id
	std::sort(
			sortedDataSet.begin(),
			sortedDataSet.end(),
			[&point](const DataObject& a, const DataObject& b) -> bool {
				float da = d(point, a.getPoint());
				float db = d(point, b.getPoint());
				return da < db || (da == db && a.getId() < b.getId());
			}
		);

	unsigned resultSize = std::min(k, (unsigned) sortedDataSet.size());
	Results results (resultSize);

	std::transform(
			sortedDataSet.begin(),
			sortedDataSet.begin() + resultSize,
			results.begin(),
			[](const DataObject& object) -> DataObject::Id {
				return object.getId();
			}
		);

	return results;
};
