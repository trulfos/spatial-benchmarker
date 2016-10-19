#include "ParallelSpatialIndex.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <vector>


ParallelSpatialIndex::ParallelSpatialIndex(const DataSet& dataSet)
{
	if (dataSet.empty()) {
		return;
	}

	// Initialize sizes
	nObjects = dataSet.size();
	dimension = dataSet[0].getPoint().getDimension();

	// Allocate buffers
	positions = new float[nObjects * dimension];
	ids = new DataObject::Id[nObjects];

	// Copy data into buffers
	for (unsigned i = 0; i < nObjects; i++) {

		ids[i] = dataSet[i].getId();

		const float * point = dataSet[i].getPoint().getCoordinates();
		for (unsigned j = 0; j < dimension; j++) {
			positions[dimension * i + j] = point[j];
		}

	}
};

ParallelSpatialIndex::~ParallelSpatialIndex()
{
	delete[] positions;
	delete[] ids;
};


ResultSet ParallelSpatialIndex::rangeSearch(const AxisAlignedBox& box) const
{
	ResultSet results;
	const float * pointA = box.getPoints().first.getCoordinates();
	const float * pointB = box.getPoints().second.getCoordinates();

	// Scan through data
#	pragma omp parallel for
	for (unsigned i = 0; i < nObjects; i++) {
		bool inside = true;

		for (unsigned j = 0; j < dimension; j++) {
			const float& coordinate = positions[dimension * i + j];
			inside &= pointA[j] <= coordinate && coordinate <= pointB[j];
		}

		if (inside) {
			//TODO: Is this a bottleneck. Alternative: Combine sets after loop
#			pragma omp critical
			results.push_back(ids[i]);
		}
	}

	return results;
};


ResultSet ParallelSpatialIndex::knnSearch(unsigned k, const Point& point) const
{
	const float * reference = point.getCoordinates();
	float maxDistance = std::numeric_limits<float>::infinity();

	// Construct priority queue
	struct QueueEntry {
		float distance;
		DataObject::Id id = 0;

		QueueEntry(float distance, DataObject::Id id)
			: distance(distance), id(id)
		{};
	};

	std::priority_queue<
			QueueEntry,
			std::vector<QueueEntry>,
			std::function<bool(const QueueEntry&, const QueueEntry&)>
		> queue (
			[](const QueueEntry& a, const QueueEntry& b) -> bool {
				return a.distance < b.distance ||
					(a.distance == b.distance && a.id < b.id);
			}
		);

	// Scan through data
#	pragma omp parallel for
	for (unsigned i = 0; i < nObjects; i++) {
		float distance = 0.0f;

		for (unsigned j = 0; j < dimension; j++) {
			float diff = positions[dimension * i + j] - reference[j];
			distance += diff * diff;
		}

		// Alternative: Create <#threads> k sets and merge the results
		if (distance <= maxDistance) {

#			pragma omp critical (queue_critical)
			if (distance <= maxDistance) {
				//TODO: check if the above (additional) check makes any
				// difference and whether it is positive
				queue.emplace(distance, ids[i]);
				if (queue.size() > k) {
					queue.pop();
					maxDistance = queue.top().distance;
				}
			}

		}
	}


	// Construct results
	ResultSet results;

	while (!queue.empty()) {
		results.push_back(queue.top().id);
		queue.pop();
	}

	return results;
};
