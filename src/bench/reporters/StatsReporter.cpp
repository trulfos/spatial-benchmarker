#include "StatsReporter.hpp"
#include "spatial/StatsCollector.hpp"
#include "ProgressLogger.hpp"

void StatsReporter::run(
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	auto queries = getQuerySet();
	ProgressLogger progress(logStream, queries.getSize());

	for (auto query : queries) {

		// Collect statistics
		StatsCollector stats;
		Results results = index.search(query, stats);

		// Store statistics
		for (auto stat : stats) {
			addEntry(stat.first, stat.second);
		}

		progress.increment();
	}
}
