#pragma once
#include "RunTimeReporter.hpp"
#include "MetricReporter.hpp"

/**
 * Reports the run time of each query.
 */
class QueryRunTimeReporter : public RunTimeReporter
{
	public:
		void run(
				const std::string& name,
				Benchmark& benchmark,
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

	protected:
		/**
		 * The run time will be measured within these constraints (in order of
		 * priority).
		 */
		static const unsigned MAX_RUNS = 10;
		static const unsigned long MIN_TOTAL_TIME = 5 * 1e6; // µs
};
