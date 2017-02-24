#include "TotalRunTimeReporter.hpp"
#include "ProgressLogger.hpp"
#include <algorithm>

void TotalRunTimeReporter::run(
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	ProgressLogger progress(logStream, RUNS);

	auto querySet = getQuerySet();

	// Load queries into memory
	std::vector<RangeQuery> queries (querySet.getSize());
	std::transform(
			querySet.begin(), querySet.end(),
			queries.begin(),
			[](const decltype(querySet)::value_type& query) {
				return query;
			}
		);

	// Test multiple times
	for (unsigned i = 0; i < RUNS; ++i) {

		// Clear cache
		clearCache();


		// Time queries
		auto startTime = clock::now();

		for (const RangeQuery& query : queries) {
			index.search(query);
		}

		auto endTime = clock::now();


		// Calculate runtime
		unsigned long long result =
			std::chrono::duration_cast<std::chrono::microseconds>(
						endTime - startTime
					).count();

		addEntry("total_runtime", result);

		// Rearrange queries
		std::vector<RangeQuery> permutation (queries.size());

		unsigned position = 0;
		for (const RangeQuery& query : queries) {
			permutation[position] = query;
			position = (position + 11) % queries.size();
		}

		queries = permutation;

		// Update progress bar
		progress.increment();

	}
}
