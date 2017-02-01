#pragma once
#include "common/Query.hpp"
#include "common/Results.hpp"
#include "common/Box.hpp"

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

	protected:
		virtual Results rangeSearch(const Box& box) const = 0;
		virtual Results knnSearch(unsigned k, const Point& point) const = 0;
};
