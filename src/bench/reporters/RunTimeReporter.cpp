#include "RunTimeReporter.hpp"
#include <algorithm>

RunTimeReporter::RunTimeReporter()
{
	if (
			clock::period::num * period::period::den
			> clock::period::den * period::period::num
	) {
		throw std::runtime_error(
			"The clock resolution is too low for the selected period"
		);
	}
}

Results RunTimeReporter::run(
		const std::string& name,
		const Query& query,
		const SpatialIndex& index
	)
{
	// Time task
	auto startTime = clock::now();
	Results results = index.search(query);
	auto endTime = clock::now();

	// Store result
	timeseries[name].push_back(
			std::chrono::duration_cast<std::chrono::microseconds>(
				endTime - startTime
			).count()
		);

	if (queries.size() < timeseries[name].size()) {
		queries.push_back(query.getName());
	}

	return results;
}

void RunTimeReporter::generate(std::ostream& stream) const
{
	std::vector<unsigned> sizes;
	std::transform(
			timeseries.begin(),
			timeseries.end(),
			std::back_inserter(sizes),
			[](const decltype(timeseries)::value_type& pair) {
				return (unsigned) pair.second.size();
			}
		);

	auto max = std::max_element(sizes.begin(), sizes.end());

	if (max == sizes.end()) {
		stream << "No data to report";
		return;
	}

	// Print header
	stream << "query\t";
	for (auto pair : timeseries) {
		stream << pair.first << '\t';
	}

	// Print data lines
	for (unsigned i = 0; i < *max; ++i) {
		stream << '\n' << queries[i];
		for (auto pair : timeseries) {
			stream << '\t';
			if (i < pair.second.size()) {
				stream << pair.second[i];
			}
		}
	}
}
