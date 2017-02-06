#pragma once
#include "MetricReporter.hpp"

/**
 * Collects and reports statistics from the benchmark.
 */
class StatsReporter : public MetricReporter
{
	public:

		Results run(
				const std::string& name,
				const Query& query,
				const SpatialIndex& index
			);

};
