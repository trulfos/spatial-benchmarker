#pragma once
#include "bench/LazyDataSet.hpp"
#include "common/SpatialIndex.hpp"
namespace Naive
{

/**
 * Simplest algorithm just for checking the results.
 *
 * The simlicity makes it easier to reason about the program such that the
 * results can be used for comparison.
 */
class SpatialIndex : public ::SpatialIndex
{

	public:
		void insert(const DataObject& object) override;

	protected:
		Results rangeSearch(const Box& box) const;
		
		Results knnSearch(unsigned k, const Point& point) const;

	private:
		std::vector<DataObject> dataSet;
};

}
