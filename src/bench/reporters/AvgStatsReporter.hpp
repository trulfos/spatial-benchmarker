#pragma once
#include "QueryReporter.hpp"

namespace Bench
{

/**
 * Collects and reports statistics from the benchmark.
 */
class AvgStatsReporter : public QueryReporter
{
	public:

		using QueryReporter::QueryReporter;

		void run(
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

};

}
