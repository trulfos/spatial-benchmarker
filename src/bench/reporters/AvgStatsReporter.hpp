#pragma once
#include "MetricReporter.hpp"

/**
 * Collects and reports statistics from the benchmark.
 */
class AvgStatsReporter : public MetricReporter
{
	public:

		using MetricReporter::MetricReporter;

		void run(
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

};
