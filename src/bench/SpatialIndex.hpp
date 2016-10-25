#pragma once
#include "../common/Query.hpp"
#include "../common/Results.hpp"
#include "../common/AxisAlignedBox.hpp"

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
		Results search(const Query& query);

	protected:
		virtual Results rangeSearch(const AxisAlignedBox& box) const = 0;
		virtual Results knnSearch(unsigned k, const Point& point) const = 0;
};
