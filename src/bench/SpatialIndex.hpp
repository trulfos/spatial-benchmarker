#pragma once
#include "common/Query.hpp"
#include "common/Results.hpp"
#include "common/Box.hpp"
#include "StatsCollector.hpp"

/**
 * This abstract class is a common interface to all spatial indexes.
 */
class SpatialIndex
{
	public:

		virtual ~SpatialIndex();

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
