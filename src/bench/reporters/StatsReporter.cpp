#include "StatsReporter.hpp"
#include "bench/StatsCollector.hpp"

Results StatsReporter::run(
		const std::string& name,
		const Query& query,
		const SpatialIndex& index
	)
{
	// Collect statistics
	StatsCollector stats;
	Results results = index.search(query, stats);

	// Store statistics
	for (auto stat : stats) {
		addEntry(query, stat.first, std::to_string(stat.second));
	}

	return results;
}
