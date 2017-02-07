#pragma once
#include "common/Coordinate.hpp"
#include "common/DataObject.hpp"
#include "bench/LazyDataSet.hpp"
#include "bench/SpatialIndex.hpp"
#include <vector>

namespace Sequential
{

/**
 * Parallel scan of all objects using OpenMP.
 */
class SpatialIndex : public ::SpatialIndex
{

	public:
		SpatialIndex() = default;
		~SpatialIndex();

		// Cannot be copied
		SpatialIndex(const SpatialIndex&) = delete;

		void load(LazyDataSet& dataSet);

	protected:
		Results rangeSearch(const Box& box) const;
		Results knnSearch(unsigned k, const Point& point) const;

	private:
		unsigned nObjects;
		unsigned dimension;
		Coordinate * positions;
		DataObject::Id * ids;
};

}
