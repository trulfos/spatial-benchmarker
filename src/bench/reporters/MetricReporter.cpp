#include "MetricReporter.hpp"

void MetricReporter::addEntry(
		const Query& query,
		const std::string& metric,
		const std::string& value
	)
{
	results.emplace_back(
			ResultEntry {query.getName(), metric, value}
		);
}


void MetricReporter::generate(std::ostream& stream) const
{
	// Print header
	stream << "query\tmetric\tvalue\n";

	// Print data
	for (const auto& r : results) {
		stream << r.query << '\t'
			<< r.metric << '\t'
			<< r.value << '\n';
	}

	stream << std::flush;
}
