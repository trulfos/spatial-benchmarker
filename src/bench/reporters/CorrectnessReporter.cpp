#include "CorrectnessReporter.hpp"
#include "bench/Zipped.hpp"
#include <algorithm>

void CorrectnessReporter::run(
		const std::string& name,
		Benchmark& benchmark,
		const SpatialIndex& index,
		std::ostream& logStream
	)
{
	auto queries = benchmark.getQueries();
	auto results = benchmark.getResults();

	if (queries.getSize() != results.size()) {
		throw std::logic_error("Result and query set differ in size");
	}

	for (auto testCase : zip(queries, results)) {
		Results results = index.search(testCase.first);

		// Sort results
		std::sort(results.begin(), results.end());

		// Compare to known results
		correct.push_back(results == testCase.second);
	}
}

void CorrectnessReporter::generate(std::ostream& stream) const
{
	stream << "index\tcorrect\n";

	unsigned i = 0;
	for (bool c : correct) {
		stream << i++ << '\t' << (c ? '1' : '0') << '\n';
	}

	stream << std::flush;
}
