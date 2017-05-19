#include "SpatialIndex.hpp"
#include <limits>
#include <queue>
#include <memory>
#include "immintrin.h"
#include "malloc.h"

namespace Vectorized
{

// Number of items per block
constexpr unsigned blockSize = sizeof(__m256) / sizeof(Coordinate);


SpatialIndex::SpatialIndex(unsigned dimension, unsigned long long size)
	: dimension(dimension)
{
	// Allocate buffers
	nBlocks = (size - 1) / blockSize + 1;

	positions = reinterpret_cast<decltype(positions)>(
			memalign(
			sizeof(__m256),
			2 * dimension * nBlocks * sizeof(__m256)
		));

	ids = new DataObject::Id[size];

}


void SpatialIndex::insert(const DataObject& object)
{
	static_assert(
			std::is_same<double, Coordinate>::value,
			"Vectorized is currently adapted for doubles"
		);

	// Copy object id
	ids[nObjects] = object.getId();

	// Insert box data
	const auto& points = object.getBox().getPoints();
	unsigned base = 2 * blockSize * dimension * (nObjects / blockSize)
			+ nObjects % blockSize;

	for (unsigned j = 0; j < dimension; j++) {
		const unsigned k = base + 2 * blockSize * j;
		positions[k] = points.first[j];
		positions[k + blockSize] = points.second[j];
	}

	nObjects++;
};

SpatialIndex::~SpatialIndex()
{
	free(positions);
	delete[] ids;
};


void SpatialIndex::rangeSearch(Results& results, const Box& box) const
{
	constexpr unsigned short mask = (1u << blockSize) - 1u;
	const auto points = box.getPoints();

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
};


void SpatialIndex::knnSearch(Results&, unsigned, const Point&) const
{
	throw std::logic_error("KNN search not implemented");
};

}
