#pragma once
#include "LazyDataSet.hpp"
#include "SpatialIndex.hpp"
/**
 * Simplest algorithm just for checking the results.
 *
 * The simlicity makes it easier to reason about the program such that the
 * results can be used for comparison.
 */
class NaiveSpatialIndex : public SpatialIndex
{

	public:
		~NaiveSpatialIndex();
		NaiveSpatialIndex(LazyDataSet& dataSet);

	protected:
		Results rangeSearch(const AxisAlignedBox& box) const;
		
		Results knnSearch(unsigned k, const Point& point) const;

	private:
		std::vector<DataObject> dataSet;
};
