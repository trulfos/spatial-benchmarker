#include "ProgressLogger.hpp"
#include <string>

ProgressLogger::ProgressLogger(std::ostream& stream, unsigned long long max)
	: stream(stream), max(max)
{
	set(progress);
}

void ProgressLogger::set(unsigned long long p)
{
	unsigned oldCount = (2 * (LENGTH - 2) * progress + max) / (2 * max);
	unsigned count = (2 * (LENGTH - 2) * p + max) / (2 * max);

	if (oldCount != count || progress == 0) {
		stream << '\r';

		for (unsigned i = 0; i < count; ++i) {
			stream << "█";
		}

		for (unsigned i = 0; i < LENGTH - 2 - count; ++i) {
			stream << "░";
		}

		stream << std::flush;
	}

	progress = p;
}

void ProgressLogger::increment()
{
	set(progress + 1);
}
