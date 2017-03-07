#pragma once
#include "Query.hpp"
#include "Results.hpp"
#include "Box.hpp"
#include "DataObject.hpp"
#include "StatsCollector.hpp"

/**
 * This abstract class is a common interface to all spatial indexes.
 */
class SpatialIndex
{
	public:

		virtual ~SpatialIndex();


		/**
		 * Set the data domain.
		 *
		 * @param bounds Data domain
		 */
		virtual void setBounds(const Box& bounds);


		/**
		 * Index an object.
		 *
		 * @param object Data object to insert
		 */
		virtual void insert(const DataObject& object) = 0;


		/**
		 * Check the structure of this index.
		 * Does nothing by default and assumes a valid structure.
		 *
		 * @return True if structure is valid.
		 */
		virtual void checkStructure() const;


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
