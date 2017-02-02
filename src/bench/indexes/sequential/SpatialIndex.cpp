#include "SpatialIndex.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <vector>

namespace Sequential
{


SpatialIndex::SpatialIndex(LazyDataSet& dataSet)
{
	// Initialize sizes
	nObjects = dataSet.getSize();
	dimension = dataSet.getDimension();

	if (!nObjects) {
		return;
	}

	// Allocate buffers
	positions = new Coordinate[2 * nObjects * dimension];
	ids = new DataObject::Id[nObjects];

	// Copy data into buffers
	unsigned i = 0;
	for (const DataObject& object : dataSet) {

		ids[i] = object.getId();

		const auto& points = object.getBox().getPoints();
		for (unsigned j = 0; j < dimension; j++) {
			positions[2 * (dimension * i + j)] = points.first[j];
			positions[2 * (dimension * i + j) + 1] = points.second[j];
		}

		i++;
	}
};

SpatialIndex::SpatialIndex()
{
	delete[] positions;
	delete[] ids;
};


Results SpatialIndex::rangeSearch(const Box& box) const
{
	Results results;
	const Point& bottom = box.getPoints().first;
	const Point& top = box.getPoints().second;

	// Scan through data
	for (unsigned i = 0; i < nObjects; i++) {
		bool intersects = true;

		for (unsigned j = 0; j < dimension; j++) {
			const unsigned k = 2 * (dimension * i + j);
			intersects &= (
					top[j] >= positions[k] && positions[k + 1] >= bottom[j]
				);
		}

		if (intersects) {
			results.push_back(ids[i]);
		}
	}

	return results;
};


Results SpatialIndex::knnSearch(unsigned k, const Point& point) const
{
	throw std::logic_error("KNN search not implemented");
};

}
