#pragma once
#include "../../common/DataObject.hpp"

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
	 * Compare for queue sorting. Note that this reverses the sort order
	 * to create a min queue instead of a max queue.
	 */
	bool operator<(const KnnQueueEntry& other) const
	{
		if (
			distance == other.distance &&
			elevation == 0 && other.elevation == 0
		) {
				return id > other.id;
		}

		return distance > other.distance;
	};
};
