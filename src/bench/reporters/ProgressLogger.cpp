#include "ProgressLogger.hpp"
#include <string>

ProgressLogger::ProgressLogger(std::ostream& stream, unsigned max)
	: stream(stream), max(max)
{
	stream << "\033[?25l"; // Hides cursor
	set(progress);
}

ProgressLogger::~ProgressLogger()
{
	stream << "\033[?25h" << std::endl;
}

void ProgressLogger::set(unsigned p)
{
	unsigned oldCount = (2 * (LENGTH - 2) * progress + max) / (2 * max);
	unsigned count = (2 * (LENGTH - 2) * p + max) / (2 * max);

	if (oldCount != count) {
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
