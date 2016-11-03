#include "SpeedupReporter.hpp"
#include <algorithm>

SpeedupReporter::SpeedupReporter(unsigned runs, std::string reference)
	: RunTimeReporter(runs), reference(reference)
{
};

void SpeedupReporter::generate(std::ostream& stream) const
{
	if (timeseries.empty() || timeseries.count(reference) == 0) {
		stream << "No report. Make sure the naive index has been included.";
		return;
	}

	const auto baseline = timeseries.at(reference);

	// Print header
	stream << "query\t";
	for (auto pair : timeseries) {
		if (pair.first != reference) {
			stream << pair.first << '\t';
		}
	}

	// Print data lines
	for (unsigned i = 0; i < baseline.size(); ++i) {
		stream << '\n' << queries[i];

		for (auto pair : timeseries) {
			if (pair.first != reference) {
				stream << '\t';
				if (i < pair.second.size()) {
					stream << (float) baseline[i] / pair.second[i];
				}
			}
		}
	}
}
