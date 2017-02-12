#include "StatsReporter.hpp"
#include "bench/StatsCollector.hpp"
#include "ProgressLogger.hpp"

void StatsReporter::run(
		const std::string& name,
		Benchmark& benchmark,
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	auto queries = benchmark.getQueries();
	ProgressLogger progress(logStream, queries.getSize());
	unsigned i = 0;

	for (auto query : queries) {

		// Collect statistics
		StatsCollector stats;
		Results results = index.search(query, stats);

		// Store statistics
		for (auto stat : stats) {
			addEntry(i, stat.first, stat.second);
		}

		++i;
		progress.increment();
	}
}
