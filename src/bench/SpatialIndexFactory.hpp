#pragma once
#include "LazyDataSet.hpp"
#include "SpatialIndex.hpp"
#include <memory>
#include <string>
#include <vector>

/**
 * Factory for spatial indexes.
 *
 * Makes it more convenient to create indexes by name and with the same input
 * for all.
 */
class SpatialIndexFactory
{
	public:

		/**
		 * Create a spatial index using the given algorithm and with the given
		 * data set.
		 */
		static std::shared_ptr<SpatialIndex> create(
				const std::string& algorithm,
				LazyDataSet& dataSet
			);

		/**
		 * Get all index keys.
		 */
		static std::vector<std::string> keys();
};
