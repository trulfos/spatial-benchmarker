#include "SpatialIndex.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <vector>
#include <memory>
#include "xmmintrin.h"


#include <iostream>

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

	positions = aligned_alloc<Coordinate>(
			4 * sizeof(Coordinate),
			4 * ((dimension * nObjects - 1) / 4 + 1) * sizeof(Coordinate),
			buffer
		);

	ids = new DataObject::Id[nObjects];

	// Copy data into buffers
	unsigned i = 0;
	for (DataObject& object : dataSet) {

		ids[i] = object.getId();

		const Point& point = object.getPoint();
		for (unsigned j = 0; j < dimension; j++) {
			positions[dimension * i + j] = point[j];
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
	Results results;
	void * refBuf;
	const auto points = box.getPoints();

	const unsigned blockSize = dimension * (
			dimension % 2 ? 4 : (dimension % 4 ? 2 : 1)
		);

	Coordinate * references =  aligned_alloc<Coordinate>(
			4 * sizeof(Coordinate),
			blockSize * 2 * sizeof(Coordinate),
			refBuf
		);


	// Copy points to aligned memory
	for (unsigned i = 0; i < blockSize; ++i) {
		references[i] = points.first[i % dimension];
		references[i + blockSize] = points.second[i % dimension];
	}


#	pragma omp parallel for
	for (unsigned i = 0; i < nObjects * dimension; i += blockSize) {

		unsigned short remaining = dimension;
		unsigned char r = 0;

		unsigned end = std::min(i + blockSize, nObjects * dimension);

		for (unsigned j = i; j < end; j += 4) {

			// Load point data
			//TODO: Shift instead of loading for small values
			__m128 bottom = _mm_load_ps(references + j - i);
			__m128 top = _mm_load_ps(references + blockSize + j - i);

			__m128 x = _mm_load_ps(positions + j);

			// Compare
			unsigned char outside = 
					_mm_movemask_ps(_mm_cmp_ps(x, top, _CMP_GT_OS)) |
					_mm_movemask_ps(_mm_cmp_ps(x, bottom, _CMP_LT_OS));

			// We may have to run further before checking the result
			if (remaining > 4) {
				r |= outside;
				remaining -= 4;
				continue;
			}

			// Finish this round
			r |= outside & ((1u << remaining) - 1);
			if (!r) {
#				pragma omp critical(results)
				results.push_back(ids[j / dimension]);
			}

			// Start next round
			r = outside & ~((1u << remaining) - 1);
			remaining = dimension + remaining - 4;

			// Some rounds we have to push twice to keep up (when d < 4)
			// TODO: Can be skipped if d >= 4.
			if (remaining == 0) {
				if (!r) {
#					pragma omp critical(results)
					results.push_back(ids[j / dimension + 1]);
				}
				r = 0;
				remaining = dimension;
			}
		}
	}

	::operator delete(refBuf);

	return results;
};


Results SpatialIndex::knnSearch(unsigned k, const Point& point) const
{
	throw std::runtime_error("knnSearch is not implemented for vectorized");
};

}
