#pragma once
#include "common/Query.hpp"
#include "common/Results.hpp"
#include "common/Box.hpp"
#include "StatsCollector.hpp"
#include "LazyDataSet.hpp"

/**
 * This abstract class is a common interface to all spatial indexes.
 */
class SpatialIndex
{
	public:

		virtual ~SpatialIndex();


		/**
		 * Index a data set.
		 *
		 * @param dataSet Data set to index
		 */
		virtual void load(LazyDataSet& dataSet) = 0;


		/**
		 * Check the structure of this index.
		 * Does nothing by default and assumes a valid structure.
		 *
		 * @return True if structure is valid.
		 */
		virtual bool checkStructure() const;


		/**
		 * Collect statistics for this index.
		 * This can be used to collect statistics concerning the index structure
		 * which are independent of the query being run.
		 *
		 * @return Statistics about the index
		 */
		virtual StatsCollector collectStatistics() const;


		/**
		 * Perform a search using the given query.
		 */
		Results search(const Query& query) const;


		/**
		 * Performs an instrumeted search.
		 *
		 * This should be the same as the normal search, but sacrifices
		 * performance to collect statistics while running.
		 */
		Results search(const Query& query, StatsCollector& collector) const;

	protected:
		virtual Results rangeSearch(const Box& box) const = 0;
		virtual Results knnSearch(unsigned k, const Point& point) const = 0;

		virtual Results rangeSearch(
				const Box& box,
				StatsCollector& collector
			) const;

		virtual Results knnSearch(
				unsigned k,
				const Point& point,
				StatsCollector& collector
			) const;
};
