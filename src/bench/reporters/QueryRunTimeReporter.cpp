#include "QueryRunTimeReporter.hpp"
#include "ProgressLogger.hpp"
#include <algorithm>
#include <limits>

namespace Bench
{

void QueryRunTimeReporter::run(
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	auto queries = getQuerySet();
	ProgressLogger progress(logStream, queries.getSize());

	for (auto query : queries) {

		unsigned runs = MAX_RUNS;
		unsigned long total = 0;
		unsigned long min = std::numeric_limits<unsigned long>::max();
		Results results;

		while (runs-- && total < MIN_TOTAL_TIME) {
			clearCache();
			Results newResults;
			newResults.reserve(MIN_RESULT_SIZE);

			// Time task
			auto startTime = clock::now();
			index.search(newResults, query);
			auto endTime = clock::now();

			// Calculate runtime
			unsigned long runtime =
				std::chrono::duration_cast<std::chrono::microseconds>(
							endTime - startTime
						).count();

			min = std::min(min, runtime);
			total += runtime;

			// Check results
			std::sort(newResults.begin(), newResults.end());

			if (!results.empty() && results != newResults) {
				throw std::runtime_error(
						"Inconsistent results from index search"
					);
			}

			results = newResults;
		}

		// TODO: Check result

		// Store result
		addEntry("query_runtime", min);

		// Update progress bar
		progress.increment();
	}
}

}
