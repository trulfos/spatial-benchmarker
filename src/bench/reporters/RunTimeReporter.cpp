#include "RunTimeReporter.hpp"

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
	const unsigned size = CACHE_SIZE * 1024;
	volatile char * buffer = new char[size];

	// Write bullshit
	for (unsigned i = 0; i < size; i += CACHE_LINE_SIZE) {
		buffer[i] = (char) i;
	}

	// Read bullshit and write it again
	for (unsigned j = 0; j < CACHE_LINE_SIZE - 1; ++j) {
		for (unsigned i = 0; i < size; i += CACHE_LINE_SIZE) {
			buffer[i + j + 1] = buffer[i + j];
		}
	}

	delete[] buffer;
}

}
