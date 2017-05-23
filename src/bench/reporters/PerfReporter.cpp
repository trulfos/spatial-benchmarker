#include "PerfReporter.hpp"
#include "ProgressLogger.hpp"
#include <algorithm>
#include <unistd.h>

namespace Bench
{

PerfReporter::PerfReporter(
		const std::string& path,
		const std::string& event,
		unsigned burn
	) : QueryReporter(path), event(event), burn(burn)
{
}

void PerfReporter::run(
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	const unsigned runs = 100;
	ProgressLogger progress(logStream, runs + burn);
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

	// Reserve space for results
	Results r;
	r.reserve(MIN_RESULT_SIZE);

	// Prepare command for later
	std::string command (
			"perf record -e" + event + " -p" + std::to_string(getpid()) + "&"
		);



	// Test multiple times
	for (unsigned i = 0; i < runs + burn; ++i) {

		if (i == burn) {
			// Go head and start perf (in background)
			std::system(command.c_str());
		}


		// Clear cache
		clearCache();

		for (const RangeQuery& query : queries) {
			r.clear();
			index.search(r, query);
		}

		// Update progress bar
		progress.increment();

	}
}

}
