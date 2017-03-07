#pragma once
#include "common/Coordinate.hpp"
#include "common/DataObject.hpp"
#include "common/SpatialIndex.hpp"

namespace Vectorized
{

/**
 * Parallel scan of all objects using OpenMP.
 */
class SpatialIndex : public ::SpatialIndex
{

	public:
		SpatialIndex(unsigned dimension, unsigned long long size);
		~SpatialIndex();

		// Cannot be copied
		SpatialIndex(const SpatialIndex&) = delete;

		void insert(const DataObject& object);

	protected:
		Results rangeSearch(const Box& box) const;
		Results knnSearch(unsigned k, const Point& point) const;

	private:
		unsigned nBlocks;
		unsigned long long nObjects = 0;
		unsigned dimension;
		void * buffer;
		Coordinate * positions;
		DataObject::Id * ids;
};

}
