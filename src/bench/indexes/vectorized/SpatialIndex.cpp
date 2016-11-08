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

	//TODO: Make sure there is 4 left at the end
	positions = aligned_alloc<Coordinate>(
			8 * sizeof(Coordinate),
			8 * ((dimension * nObjects - 1) / 8 + 1) * sizeof(Coordinate),
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
	Results results;
	const unsigned blockSize = sizeof(block) * 8;
	const unsigned nBlocks = (nObjects - 1)/blockSize + 1;

	block * resultVector = new std::uint32_t[nBlocks]();


#	pragma omp parallel
	{
		unsigned d = std::numeric_limits<unsigned>::max();
		__m256 bottom;
		__m256 top;

#		pragma omp for schedule(static)
		for (unsigned b = 0; b < dimension * nBlocks; ++b) {
			if (d != b / nBlocks) {
				d = b / nBlocks;
				bottom = _mm256_broadcast_ss(&points.first[d]);
				top = _mm256_broadcast_ss(&points.second[d]);
			}

			block temporary = 0;

			// One SIMD block at a time
			for (unsigned j = 0; j < blockSize; j += 8) {

				__m256 x = _mm256_load_ps(positions + b * blockSize + j);

				block outside = 
						_mm256_movemask_ps(_mm256_cmp_ps(x, top, _CMP_GT_OS)) |
						_mm256_movemask_ps(_mm256_cmp_ps(x, bottom, _CMP_LT_OS));

				temporary |= (outside << j);
			}

#			pragma omp atomic
			resultVector[b % nBlocks] |= temporary;
		}

#		pragma omp barrier

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
	throw std::runtime_error("knnSearch is not implemented for vectorized");
};

}
