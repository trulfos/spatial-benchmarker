#include "MetricReporter.hpp"

void MetricReporter::addEntry(
		unsigned index,
		const std::string& name,
		value_type value
	)
{
	results.emplace_back(
			ResultEntry {name, value}
		);
}


void MetricReporter::generate(std::ostream& stream) const
{
	// Print header
	stream << "name\tvalue\n";

	// Print data
	for (const auto& r : results) {
		stream << r.name << '\t'
			<< r.value << '\n';
	}

	stream << std::flush;
}
