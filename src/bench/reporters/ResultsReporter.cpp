#include "ResultsReporter.hpp"
#include <algorithm>
#include "ProgressLogger.hpp"

void ResultsReporter::run(
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	auto queries = getQuerySet();
	ProgressLogger progress(logStream, queries.getSize());

	for (auto query : queries) {

		resultSet.push_back(
				index.search(query)
			);

		std::sort(resultSet.back().begin(), resultSet.back().end());

		progress.increment();
	}
}


void ResultsReporter::generate(std::ostream& stream) const
{
	stream << resultSet;
}
