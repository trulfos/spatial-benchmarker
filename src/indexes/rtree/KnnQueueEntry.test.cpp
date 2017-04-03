#include <criterion/criterion.h>
#include "KnnQueueEntry.hpp"

using namespace Rtree;

using KQE = KnnQueueEntry<void>;
KQE makeEntry(unsigned elevation, float distance)
{
	return KQE(nullptr, elevation, distance);
}


Test(knnQueueEntry, distance_sort)
{
	cr_expect(
			makeEntry(0, 2.0f) > makeEntry(0, 1.5f),
			"A distance of 2.0f should be greater than one of 1.5f"
		);
}


Test(knnQueueEntry, elevation_sort)
{
	cr_expect(
			makeEntry(2, 2.0f) > makeEntry(3, 2.0f),
			"An entry at elevation 2 should be greater than one at elevation 3"
		);
}


Test(knnQueryEntry, elevation_sort)
{
	cr_expect(
			KQE(2u, 0, 0.0f) > KQE(1u, 0, 0.0f),
			"IDs should decide ties"
		);
}
