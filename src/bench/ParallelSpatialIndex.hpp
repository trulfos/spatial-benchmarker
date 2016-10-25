#pragma once
#include "../common/Coordinate.hpp"
#include "../common/DataObject.hpp"
#include "../common/DataSet.hpp"
#include "SpatialIndex.hpp"
#include <vector>
/**
 * Parallel scan of all objects using OpenMP.
 */
class ParallelSpatialIndex : public SpatialIndex
{

	public:
		~ParallelSpatialIndex();
		ParallelSpatialIndex(const DataSet& dataSet);

		// Cannot be copied
		ParallelSpatialIndex(const ParallelSpatialIndex&) = delete;

	protected:
		Results rangeSearch(const AxisAlignedBox& box) const;
		Results knnSearch(unsigned k, const Point& point) const;

	private:
		unsigned nObjects;
		unsigned dimension;
		Coordinate * positions;
		DataObject::Id * ids;
};
