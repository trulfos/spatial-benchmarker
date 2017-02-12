#include "MetricReporter.hpp"

void MetricReporter::addEntry(
		unsigned index,
		const std::string& metric,
		value_type value
	)
{
	results.emplace_back(
			ResultEntry {index, metric, value}
		);
}


void MetricReporter::generate(std::ostream& stream) const
{
	// Print header
	stream << "index\tmetric\tvalue\n";

	// Print data
	for (const auto& r : results) {
		stream << r.index << '\t'
			<< r.metric << '\t'
			<< r.value << '\n';
	}

	stream << std::flush;
}
