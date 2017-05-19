#include "StatsReporter.hpp"
#include "spatial/StatsCollector.hpp"
#include "ProgressLogger.hpp"

namespace Bench
{

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
		index.search(stats, query);

		// Store statistics
		for (auto stat : stats) {
			addEntry(stat.first, stat.second);
		}

		progress.increment();
	}
}

}
