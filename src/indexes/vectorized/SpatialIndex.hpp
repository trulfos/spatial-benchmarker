#pragma once
#include "spatial/Coordinate.hpp"
#include "spatial/DataObject.hpp"
#include "spatial/SpatialIndex.hpp"

using namespace Spatial;

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
		void rangeSearch(Results& results, const Box& box) const;
		void knnSearch(Results& results, unsigned k, const Point& point) const;

	private:
		unsigned nBlocks;
		unsigned long long nObjects = 0;
		unsigned dimension;
		Coordinate * positions;
		DataObject::Id * ids;
};

}
