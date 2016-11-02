#include "ResultsReporter.hpp"
#include <algorithm>

Results ResultsReporter::run(
		const std::string& name,
		const Query& query,
		const SpatialIndex& index
	)
{
	if (reference.empty()) {
		reference = name;
	}

	Results results = index.search(query);


	if (name == reference) {
		resultSet.push_back(results);
		std::sort(resultSet.back().begin(), resultSet.back().end());
	}

	return results;
}


void ResultsReporter::generate(std::ostream& stream) const
{
	stream << resultSet;
}
