#pragma once
#include "RunTimeReporter.hpp"

/**
 * This reporter loads all queries to memory and executes them all while
 * recording the time. To avoid caching problems, the queries are randomly
 * reordered several times and rerun, giving an average runtime per query.
 *
 * This saves time when evaluating the index.
 */
class TotalRunTimeReporter : public RunTimeReporter
{
	public:
		void run(
				const std::string& name,
				Benchmark& benchmark,
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

	private:
		static constexpr unsigned RUNS = 10;

};
