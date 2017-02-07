#pragma once
#include "common/Coordinate.hpp"
#include "common/DataObject.hpp"
#include "bench/LazyDataSet.hpp"
#include "bench/SpatialIndex.hpp"
#include <vector>

namespace Vectorized
{

/**
 * Parallel scan of all objects using OpenMP.
 */
class SpatialIndex : public ::SpatialIndex
{

	public:
		SpatialIndex() = default;
		SpatialIndex(LazyDataSet& dataSet);
		~SpatialIndex();

		// Cannot be copied
		SpatialIndex(const SpatialIndex&) = delete;

	protected:
		Results rangeSearch(const Box& box) const;
		Results knnSearch(unsigned k, const Point& point) const;

	private:
		unsigned nBlocks;
		unsigned nObjects;
		unsigned dimension;
		void * buffer;
		Coordinate * positions;
		DataObject::Id * ids;
};

}
