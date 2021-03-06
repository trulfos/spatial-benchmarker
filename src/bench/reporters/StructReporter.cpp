#include "StructReporter.hpp"

namespace Bench
{

void StructReporter::run(const SpatialIndex& index, std::ostream& logStream)
{
	StatsCollector stats = index.collectStatistics();

	for (auto s : stats) {
		addEntry(s.first, s.second);
	}
}

}
