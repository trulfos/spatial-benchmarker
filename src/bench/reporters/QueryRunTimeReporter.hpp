#pragma once
#include "RunTimeReporter.hpp"
#include "QueryReporter.hpp"

/**
 * Reports the run time of each query.
 */
class QueryRunTimeReporter : public QueryReporter, private RunTimeReporter
{
	public:

		using QueryReporter::QueryReporter;

		void run(
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
