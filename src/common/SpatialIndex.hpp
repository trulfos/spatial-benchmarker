#pragma once
#include "Query.hpp"
#include "Results.hpp"
#include "Box.hpp"
#include "DataObject.hpp"
#include "StatsCollector.hpp"

/**
 * This abstract class is a common interface to all spatial indexes.
 *
 * The data to be indexed is usually inserted after the index has been created.
 * Calling the prepare method before searching allows the index to digest the
 * data in whatever way seems reasonable.
 */
class SpatialIndex
{
	public:

		virtual ~SpatialIndex();

		/**
		 * Insert an object in the index.
		 *
		 * The object is not required to be available for search before after
		 * the next call to prepare. It may thus be stored temporarily outside
		 * the actual index structure (depending on the implementation).
		 *
		 * @param object Data object to insert
		 */
		virtual void insert(const DataObject& object) = 0;


		/**
		 * Check the structure of this index.
		 *
		 * Does nothing by default and thus assumes a valid structure.
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
		 * Prepare the index for searching.
		 *
		 * Request that this index prepares for searching. Need only be run
		 * before the first search after an insert.
		 */
		virtual void prepare();


		/**
		 * Perform a search.
		 *
		 * Search for results matching the given query. Run the prepare method
		 * first to ensure the index is ready for searching.
		 *
		 * @param query Query to use for searching
		 * @return Results matching query
		 */
		Results search(const Query& query) const;


		/**
		 * Performs an instrumeted search.
		 *
		 * This should be the same as the normal search, but sacrifices
		 * performance to collect statistics while running.
		 *
		 * @param query Query to use for searching
		 * @param collector Object in which statistics should be recorded
		 * @return Results matching query
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
