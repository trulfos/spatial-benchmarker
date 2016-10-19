#pragma once
#include "../common/DataSet.hpp"
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
		 * Initialize this spatial index factory with the given data.
		 */
		SpatialIndexFactory(const DataSet& dataSet);

		/**
		 * Create a spatial index using the given algorithm.
		 */
		std::shared_ptr<SpatialIndex> create(const std::string& algorithm);

	private:
		const DataSet& dataSet;
};
