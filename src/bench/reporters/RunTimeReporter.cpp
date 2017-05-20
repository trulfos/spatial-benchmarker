#include "RunTimeReporter.hpp"
#include "mmap/MemoryMap.hpp"

namespace Bench
{

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


void RunTimeReporter::clearCache()
{
	MMap::MemoryMap map;

	for (auto region : map) {
		if (region.isReadable() && !region.isVvar()) {
			region.invalidate();
		}
	}
}

}
