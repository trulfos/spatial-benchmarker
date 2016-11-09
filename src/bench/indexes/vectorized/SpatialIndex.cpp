#include "SpatialIndex.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <vector>
#include <memory>
#include "xmmintrin.h"
#include <cstdint>

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
	if (dataSet.empty()) {
		return;
	}

	// Initialize sizes
	nObjects = dataSet.size();
	dimension = dataSet.dimension();

	// Allocate buffers
	static_assert(
			std::is_same<float, Coordinate>::value,
			"Vectorized is currently adapted for floats"
		);

	//TODO: Fix allocation size
	positions = aligned_alloc<Coordinate>(
			8 * sizeof(Coordinate),
			8 * ((dimension * nObjects - 1) / 8 + 1 + dimension * 7) * sizeof(Coordinate),
			buffer
		);

	ids = new DataObject::Id[nObjects];

	// Copy data into buffers
	unsigned i = 0;
	for (DataObject& object : dataSet) {

		ids[i] = object.getId();

		const Point& point = object.getPoint();
		for (unsigned j = 0; j < dimension; j++) {
			positions[8 * (dimension * (i / 8) + j) + i % 8] = point[j];
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
	using block = std::uint32_t;

	const auto points = box.getPoints();
	Results results;
	const unsigned blockSize = sizeof(block) * 8;
	const unsigned nBlocks = (nObjects - 1)/blockSize + 1;

	block * resultVector = new std::uint32_t[nBlocks]();


#	pragma omp parallel
	{
		__m256 bottom;
		__m256 top;

#		pragma omp for schedule(static)
		for (unsigned b = 0; b < ((nObjects - 1) / 8) + 1; ++b) {

			block temporary = 0;

			// One SIMD block at a time
			for (unsigned j = 0; j < dimension; ++j) {

				bottom = _mm256_broadcast_ss(&points.first[j]);
				top = _mm256_broadcast_ss(&points.second[j]);

				__m256 x = _mm256_load_ps(positions + 8 * (b * dimension + j));

				block outside = 
						_mm256_movemask_ps(_mm256_cmp_ps(x, top, _CMP_GT_OS)) |
						_mm256_movemask_ps(_mm256_cmp_ps(x, bottom, _CMP_LT_OS));

				temporary |= outside;
			}

#			pragma omp atomic
			resultVector[b / 4] |= (temporary << 8 * (b % 4));
		}

		// Calculate result
#		pragma omp for schedule(static)
		for (unsigned i = 0; i < nBlocks; ++i) {
			block r = resultVector[i];

			for (unsigned j = 0; j < blockSize; ++j) {
				unsigned index = blockSize * i + j;

				if (index < nObjects && (1 & ~r)) {
#					pragma omp critical
					results.push_back(ids[index]);
				}
				r = r >> 1;
			}
		}
	}

	delete[] resultVector;
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
	for (unsigned b = 0; b < nObjects / 8; ++b) {

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
