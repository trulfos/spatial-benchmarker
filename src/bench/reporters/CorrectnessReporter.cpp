#include "CorrectnessReporter.hpp"
#include "bench/Zipped.hpp"
#include "ProgressLogger.hpp"
#include <algorithm>

namespace Bench
{

CorrectnessReporter::CorrectnessReporter(
		const std::string& queryPath,
		const std::string& resultsPath
	) : QueryReporter(queryPath), resultsPath(resultsPath)
{
}

void CorrectnessReporter::run(
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	auto queries = getQuerySet();
	auto results = getResults();

	if (queries.getSize() != results.size()) {
		throw std::logic_error("Result and query set differ in size");
	}

	ProgressLogger progress (logStream, results.size());

	unsigned i = 0;

	for (auto testCase : zip(queries, results)) {
		Results results = index.search(testCase.first);

		// Sort results
		std::sort(results.begin(), results.end());

		// Compare to known results
		if (results != testCase.second) {
			incorrect.push_back(i);
		}

		++i;
		progress.increment();
	}
}

void CorrectnessReporter::generate(std::ostream& stream) const
{
	stream << "name\tvalue\n";

	for (unsigned c : incorrect) {
		stream << c << "\t1\n";
	}

	stream << std::flush;
}

ResultSet CorrectnessReporter::getResults()
{
	std::ifstream stream (resultsPath);

	if (!stream) {
		throw std::runtime_error("Could not read file " + resultsPath);
	}

	ResultSet results;
	stream >> results;

	return results;
}

}
