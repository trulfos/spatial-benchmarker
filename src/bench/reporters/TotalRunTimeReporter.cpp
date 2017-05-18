#include "TotalRunTimeReporter.hpp"
#include "ProgressLogger.hpp"
#include <algorithm>
#include <random>

namespace Bench
{

TotalRunTimeReporter::TotalRunTimeReporter(
		const std::string& queryPath,
		unsigned runs
	) : QueryReporter(queryPath), runs(runs)
{
}

void TotalRunTimeReporter::run(
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	ProgressLogger progress(logStream, runs);
	std::default_random_engine engine (11);

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
	for (unsigned i = 0; i < runs; ++i) {

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
		std::shuffle(
				queries.begin(), queries.end(),
				engine
			);

		// Update progress bar
		progress.increment();

	}
}

}
