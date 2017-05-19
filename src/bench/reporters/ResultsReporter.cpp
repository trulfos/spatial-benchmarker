#include "ResultsReporter.hpp"
#include <algorithm>
#include "ProgressLogger.hpp"

namespace Bench
{

void ResultsReporter::run(
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	auto queries = getQuerySet();
	ProgressLogger progress(logStream, queries.getSize());

	for (auto query : queries) {

		Results r;
		index.search(r, query);
		resultSet.push_back(r);

		std::sort(resultSet.back().begin(), resultSet.back().end());

		progress.increment();
	}
}


void ResultsReporter::generate(std::ostream& stream) const
{
	stream << resultSet;
}

}
