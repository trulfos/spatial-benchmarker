#pragma once
#include "QueryReporter.hpp"

/**
 * Collects and reports statistics from the benchmark.
 */
class StatsReporter : public QueryReporter
{
	public:

		using QueryReporter::QueryReporter;

		void run(
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

};
