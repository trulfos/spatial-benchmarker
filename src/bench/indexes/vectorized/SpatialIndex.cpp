#include "SpatialIndex.hpp"
#include <limits>
#include <queue>
#include <memory>
#include "immintrin.h"

namespace Vectorized
{

// Number of items per block
constexpr unsigned blockSize = sizeof(__m256) / sizeof(Coordinate);


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
	// Initialize sizes
	dimension = dataSet.getDimension();
	nObjects = dataSet.getSize();

	// Shortcuts to avoid handling edge cases
	if (!nObjects) {
		throw std::logic_error("Vectorized needs at least one block");
	}

	static_assert(
			std::is_same<double, Coordinate>::value,
			"Vectorized is currently adapted for doubles"
		);


	// Allocate buffers
	nBlocks = (nObjects - 1) / blockSize + 1;

	positions = aligned_alloc<Coordinate>(
			sizeof(__m256),
			2 * dimension * nBlocks * sizeof(__m256),
			buffer
		);

	ids = new DataObject::Id[nObjects];

	// Copy data into buffers
	unsigned i = 0;
	for (DataObject object : dataSet) {

		ids[i] = object.getId();

		const auto& points = object.getBox().getPoints();
		unsigned base = 2 * blockSize * dimension * (i / blockSize)
				+ i % blockSize;

		for (unsigned j = 0; j < dimension; j++) {
			const unsigned k = base + 2 * blockSize * j;
			positions[k] = points.first[j];
			positions[k + blockSize] = points.second[j];
		}

		i++;
	}
};

SpatialIndex::SpatialIndex()
{
	::operator delete(buffer);
	delete[] ids;
};


Results SpatialIndex::rangeSearch(const Box& box) const
{
	constexpr unsigned short mask = (1u << blockSize) - 1u;
	const auto points = box.getPoints();
	Results results;

#	pragma omp parallel for schedule(static)
	for (unsigned b = 0; b < nBlocks; ++b) {

		// Bit vector where a 1 means the object is outside
		unsigned short outside = 0;

		// Compare across all dimensions
		for (unsigned j = 0; j < dimension; ++j) {

			// Skip if we know the result
			if (!(~outside & mask)) {
				break;
			}

			// Load bottom and top for query box
			__m256d bottom = _mm256_broadcast_sd(&points.first[j]);
			__m256d top = _mm256_broadcast_sd(&points.second[j]);

			// Load bottom and top for subject
			__m256d sbottom = _mm256_load_pd(positions + 2 * blockSize * (b * dimension + j));
			__m256d stop = _mm256_load_pd(positions + 2 * blockSize * (b * dimension + j) + blockSize);

			outside |= _mm256_movemask_pd(_mm256_cmp_pd(top, sbottom, _CMP_LT_OS)) |
					_mm256_movemask_pd(_mm256_cmp_pd(stop, bottom, _CMP_LT_OS));
		}

		// Skip the rest if none were within
		if (!(~outside & mask)) {
			continue;
		}

		// Push results
		unsigned baseIndex = b * blockSize;
		for (unsigned j = 0; j < blockSize; j++) {
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
	throw std::logic_error("KNN search not implemented");
};

}
