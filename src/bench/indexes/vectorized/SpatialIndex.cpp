#include "SpatialIndex.hpp"
#include <limits>
#include <queue>
#include <memory>
#include "immintrin.h"

namespace Vectorized
{


/**
 * Allocates aligned memory.
 *
 * Remember to free the memory using the buffer pointer, not the returned
 * pointer. Also note that the space will not be initialized.
 *
 * @param aligment Must be power of 2
 * @param size Size of memory area to allocate
 * @param buffer Will hold pointer to original buffer after allocation
 */
template <typename T>
T * aligned_alloc(std::size_t alignment, std::size_t size, void *& buffer)
{
	size_t total = size + alignment - 1;
	buffer = ::operator new (total);
	uintptr_t aligned = (
			reinterpret_cast<uintptr_t>(buffer) - 1u + alignment
		) & (-alignment);

	return reinterpret_cast<T *>(aligned);
}


SpatialIndex::SpatialIndex(LazyDataSet& dataSet)
{
	// Shortcuts to avoid handling edge cases
	if (dataSet.empty()) {
		throw std::logic_error("Vectorized needs at least one block");
	}

	static_assert(
			std::is_same<float, Coordinate>::value,
			"Vectorized is currently adapted for floats"
		);

	// Initialize sizes
	nObjects = dataSet.size();
	dimension = dataSet.dimension();

	// Allocate buffers
	nBlocks = (nObjects - 1) / 8 + 1;

	positions = aligned_alloc<Coordinate>(
			sizeof(__m256),
			dimension * nBlocks * sizeof(__m256),
			buffer
		);

	ids = new DataObject::Id[nObjects];

	// Copy data into buffers
	unsigned i = 0;
	for (DataObject& object : dataSet) {

		ids[i] = object.getId();

		const Point& point = object.getPoint();
		unsigned base = 8 * dimension * (i / 8) + i % 8;

		for (unsigned j = 0; j < dimension; j++) {
			positions[base + 8 * j] = point[j];
		}

		i++;
	}
};

SpatialIndex::SpatialIndex()
{
	::operator delete(buffer);
	delete[] ids;
};


Results SpatialIndex::rangeSearch(const AxisAlignedBox& box) const
{
	const auto points = box.getPoints();
	Results results;

#	pragma omp parallel for schedule(static)
	for (unsigned b = 0; b < nBlocks; ++b) {

		// Bit vector where a 1 means the object is outside
		unsigned short outside = 0;

		// Compare across all dimensions
		for (unsigned j = 0; j < dimension; ++j) {

			// Skip if we know the result
			if (!(~outside & 0xff)) {
				break;
			}

			__m256 bottom = _mm256_broadcast_ss(&points.first[j]);
			__m256 top = _mm256_broadcast_ss(&points.second[j]);

			__m256 x = _mm256_load_ps(positions + 8 * (b * dimension + j));

			outside |= _mm256_movemask_ps(_mm256_cmp_ps(x, top, _CMP_GT_OS)) |
					_mm256_movemask_ps(_mm256_cmp_ps(x, bottom, _CMP_LT_OS));
		}

		// Skip the rest if none were within
		if (!(~outside & 0xff)) {
			continue;
		}

		// Push results
		unsigned baseIndex = b * 8;
		for (unsigned j = 0; j < 8; j++) {
			unsigned index = baseIndex + j;

			if (((outside >> j) & 1) || index >= nObjects) {
				continue;
			}

#			pragma omp critical
			results.push_back(ids[index]);
		}
	}

	return results;
};


Results SpatialIndex::knnSearch(unsigned k, const Point& point) const
{
	float best = std::numeric_limits<float>::infinity();

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

#	pragma omp parallel for schedule(static)
	for (unsigned b = 0; b < nBlocks; ++b) {

		// Sum up contributions from each dimension
		__m256 sum = _mm256_setzero_ps();
		for (unsigned d = 0; d < dimension; ++d) {
			__m256 x = _mm256_load_ps(positions + 8 * (b * dimension + d));
			__m256 y = _mm256_broadcast_ss(&point[d]);
			__m256 diff = _mm256_sub_ps(x, y);
			sum = _mm256_fmadd_ps(diff, diff, sum);
		}

		// Compare with best so far
		unsigned isBetter = _mm256_movemask_ps(
				_mm256_cmp_ps(
					sum,
					_mm256_broadcast_ss(&best),
					_CMP_LE_OS
				)
			);

		// Skip the rest if not better
		if (!isBetter) {
			continue;
		}

		float distances[8];
		_mm256_store_ps(distances, sum);

		// Filter and push to queue
		for (unsigned s = 0; s < 8; ++s) {

			if (!((isBetter >> s) & 1)) {
				continue;
			}

#			pragma omp critical (vector_queue_critical)
			if (distances[s] < best && (8 * b + s) < nObjects) {
				queue.emplace(distances[s], ids[8 * b + s]);
				if (queue.size() > k) {
					queue.pop();
					best = queue.top().distance;
				}
			}

		}
	}

	// Construct results
	Results results;

	while (!queue.empty()) {
		results.push_back(queue.top().id);
		queue.pop();
	}

	return results;
};

}
