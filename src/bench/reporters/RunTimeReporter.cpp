#include "RunTimeReporter.hpp"
#include <algorithm>
#include <limits>

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
	unsigned long min = std::numeric_limits<unsigned long>::max();
	unsigned runs = MAX_RUNS;
	unsigned long total = 0;
	Results results;

	while (runs-- && total < MIN_TOTAL_TIME) {
		clearCache();

		// Time task
		auto startTime = clock::now();
		Results newResults = index.search(query);
		auto endTime = clock::now();

		std::sort(newResults.begin(), newResults.end());

		if (!results.empty() && results != newResults) {
			throw std::runtime_error("Inconsistent results from index search");
		}

		results = newResults;

		unsigned long runtime =
			std::chrono::duration_cast<std::chrono::microseconds>(
						endTime - startTime
					).count();

		min = std::min(min, runtime);
		total += runtime;
	}

	// Store result
	timeseries[name].push_back(min);

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


void RunTimeReporter::clearCache()
{
	const unsigned size = CACHE_SIZE * 1024;
	volatile char * buffer = new char[size];

	// Write bullshit
	for (unsigned i = 0; i < size; i += CACHE_LINE_SIZE) {
		buffer[i] = (char) i;
	}

	// Read bullshit and write it again
	for (unsigned j = 0; j < CACHE_LINE_SIZE - 1; ++j) {
		for (unsigned i = 0; i < size; i += CACHE_LINE_SIZE) {
			buffer[i + j + 1] = buffer[i + j];
		}
	}

	delete[] buffer;
}
