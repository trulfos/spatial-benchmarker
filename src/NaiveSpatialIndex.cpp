#include "NaiveSpatialIndex.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath>

//TODO: Remove
#include <iostream>


/**
 * Calculates the distance between two points.
 *
 * TODO: Add to point class instead?
 *       sqrt((point1 - point2)^2)
 */
float d(const Point& a, const Point& b) {
	unsigned dimension = a.getDimension();

	if (dimension != b.getDimension()) {
		throw std::invalid_argument(
			"Cannot calculate distance between points of different dimension"
		);
	}

	float d2 = 0;
	const float * ca = a.getCoordinates();
	const float * cb = b.getCoordinates();

	for (unsigned i = 0; i < a.getDimension(); ++i) {
		float diff = ca[i] - cb[i];
		d2 += diff * diff;
	}

	return sqrt(d2);
}


/**
 * Check if the point is within the box.
 *
 * TODO: Move to box?
 */
bool contains(const AxisAlignedBox& box, const Point& point)
{
	auto points = box.getPoints();
	unsigned d = point.getDimension();

	if (
			points.first.getDimension() != d ||
			points.second.getDimension() != d
	) {
		throw std::invalid_argument(
			"Cannot check containment for point and box of different dimension"
		);
	}

	bool isWithin = true;
	for (unsigned i = 0; i < point.getDimension(); i++) {
		isWithin &=
			points.first.getCoordinates()[i] <= point.getCoordinates()[i] &&
			point.getCoordinates()[i] <= points.second.getCoordinates()[i];
	}

	return isWithin;
}


NaiveSpatialIndex::NaiveSpatialIndex(const DataSet& dataSet) : dataSet(dataSet)
{
};

NaiveSpatialIndex::~NaiveSpatialIndex()
{
};


ResultSet NaiveSpatialIndex::rangeSearch(const AxisAlignedBox& box) const
{
	std::vector<DataObject> results;

	// Filter items
	std::copy_if(
			dataSet.begin(),
			dataSet.end(),
			std::back_inserter(results),
			[&](const DataObject& object) -> bool {
				return contains(box, object.getPoint());
			}
		);

	// Extract ids
	ResultSet resultSet (results.size());
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

ResultSet NaiveSpatialIndex::knnSearch(unsigned k, const Point& point) const
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
	ResultSet results (resultSize);

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
