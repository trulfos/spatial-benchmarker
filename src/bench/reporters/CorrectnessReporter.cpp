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
	}
}

void CorrectnessReporter::generate(std::ostream& stream) const
{
	stream << "index\tincorrect\n";

	for (unsigned c : incorrect) {
		stream << c << "\t1\n";
	}

	stream << std::flush;
}
