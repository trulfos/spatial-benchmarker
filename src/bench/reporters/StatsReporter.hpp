#pragma once
#include "MetricReporter.hpp"

/**
 * Collects and reports statistics from the benchmark.
 */
class StatsReporter : public MetricReporter
{
	public:

		void run(
				const std::string& name,
				Benchmark& benchmark,
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

};
