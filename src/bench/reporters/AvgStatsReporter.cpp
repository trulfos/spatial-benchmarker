#include "AvgStatsReporter.hpp"
#include "bench/StatsCollector.hpp"
#include "ProgressLogger.hpp"

void AvgStatsReporter::run(
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	auto queries = getQuerySet();
	ProgressLogger progress(logStream, queries.getSize());
	StatsCollector averages;
	unsigned i = 0;

	for (auto query : queries) {

		// Collect statistics
		StatsCollector stats;
		index.search(query, stats);

		// Accumulate
		if (!i) {
			averages = stats;
		} else {
			for (auto stat : stats) {
				if (!averages.count(stat.first)) {
					throw std::runtime_error(
							"Statistic " + stat.first +
							" not reported for query " + std::to_string(i)
						);
				}

				averages[stat.first] += stat.second;
			}
		}

		++i;
		progress.increment();
	}

	// Record averages
	for (auto stat : averages) {
		addEntry(0, stat.first, double(stat.second) / i);
	}
}
