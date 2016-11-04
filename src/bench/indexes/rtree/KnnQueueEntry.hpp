#pragma once
#include "common/DataObject.hpp"

/**
 * Entry used in the queue for knn search using the optimal algorithm.
 */
template <typename N>
struct KnnQueueEntry {
	using Id = DataObject::Id;

	union {
		N * node;
		Id id;
	};
	unsigned elevation;
	float distance;

	KnnQueueEntry(Id id, unsigned elevation, float d)
		: id(id), elevation(elevation), distance(d) {};

	KnnQueueEntry(N * node, unsigned elevation, float d)
		: node(node), elevation(elevation), distance(d) {};

	/**
	 */
	bool operator>(const KnnQueueEntry& other) const
	{
		if (distance == other.distance) {
			if (elevation == 0 && other.elevation == 0) {
				return id > other.id;
			}

			// Prioritize blocks over objects - necessary to get correct order
			return elevation < other.elevation;
		}

		return distance > other.distance;
	};
};
