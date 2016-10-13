#pragma once
#include "AxisAlignedBox.hpp"
#include "ResultSet.hpp"
#include "Query.hpp"

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
		ResultSet search(const Query& query);

	protected:
		virtual ResultSet rangeSearch(const AxisAlignedBox& box) const = 0;
		virtual ResultSet knnSearch(unsigned k, const Point& point) const = 0;
};
