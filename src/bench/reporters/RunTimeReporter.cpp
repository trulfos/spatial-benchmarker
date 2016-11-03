#include "RunTimeReporter.hpp"
#include <algorithm>
#include <limits>

RunTimeReporter::RunTimeReporter(unsigned runs)
	: runs(runs)
{
	if (
			clock::period::num * period::period::den
			> clock::period::den * period::period::num
	) {
		throw std::runtime_error(
				"The clock resolution is too low for the selected period"
			);
	}

	if (runs == 0) {
		throw std::logic_error(
				"Measuring the minimum run time over 0 runs makes no sense"
			);
	}
}

Results RunTimeReporter::run(
		const std::string& name,
		const Query& query,
		const SpatialIndex& index
	)
{
	const unsigned runs = 10;
	unsigned long min = std::numeric_limits<unsigned long>::max();

	Results results;

	for (unsigned i = 0; i < runs; ++i) {
		clearCache();

		// Time task
		auto startTime = clock::now();
		Results newResults = index.search(query);
		auto endTime = clock::now();

		if (i > 0 && results != newResults) {
			throw std::runtime_error("Unconsistent results from index search");
		}

		results = newResults;

		min = std::min(
				min,
				(unsigned long) std::chrono::duration_cast<std::chrono::microseconds>(
						endTime - startTime
					).count()
			);
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
	unsigned size = CACHE_SIZE * 1024;
	char * buffer = new char[CACHE_SIZE * 1024];

	// write bullshit
	for (unsigned i = 0; i < size; i += CACHE_LINE_SIZE) {
		buffer[i] = (char) i;
	}

	delete[] buffer;
}
