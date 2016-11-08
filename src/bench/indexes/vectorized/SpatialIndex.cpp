#include "SpatialIndex.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <vector>
#include <memory>
#include "xmmintrin.h"
#include <cstdint>


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

	//TODO: Make sure there is 4 left at the end
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
			positions[i + j * nObjects] = point[j];
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
	const unsigned blockSize = sizeof(block) * 8;
	const unsigned nBlocks = (nObjects - 1)/blockSize + 1;

	block * resultVector = new std::uint32_t[nBlocks]();

	// Loop through each dimension
#	pragma omp parallel for
	for (unsigned d = 0; d < dimension; d++) {
		// Max and min points
		__m128 bottom = _mm_broadcast_ss(&points.first[d]);
		__m128 top = _mm_broadcast_ss(&points.second[d]);

		// Calculate one block at a time
		for (unsigned i = 0; i < nBlocks; ++i) {
			block temporary = 0;

			// One SIMD block at a time
			for (unsigned j = 0; j < blockSize; j += 4) {

				__m128 x = _mm_load_ps(positions + d * nObjects + i * blockSize + j);

				block outside = 
						_mm_movemask_ps(_mm_cmp_ps(x, top, _CMP_GT_OS)) |
						_mm_movemask_ps(_mm_cmp_ps(x, bottom, _CMP_LT_OS));

				temporary |= (outside << j);
			}

#			pragma omp atomic
			resultVector[i] |= temporary;
		}
	}

	// Calculate result
	Results results;
	for (unsigned i = 0; i < nBlocks; ++i) {
		block r = resultVector[i];

		for (unsigned j = 0; j < blockSize; ++j) {
			unsigned index = blockSize * i + j;

			if (index < nObjects && (1 & ~r)) {
				results.push_back(ids[index]);
			}
			r = r >> 1;
		}
	}

	//::operator delete(refBuf);
	delete[] resultVector;

	return results;
};


Results SpatialIndex::knnSearch(unsigned k, const Point& point) const
{
	throw std::runtime_error("knnSearch is not implemented for vectorized");
};

}
