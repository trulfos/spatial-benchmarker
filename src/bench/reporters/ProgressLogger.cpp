#include "ProgressLogger.hpp"
#include <string>

ProgressLogger::ProgressLogger(std::ostream& stream, unsigned max)
	: stream(stream), max(max)
{
	stream << "\033[?25l"; // Hides cursor
	draw();
}

ProgressLogger::~ProgressLogger()
{
	stream << "\033[?25h" << std::endl;
}

void ProgressLogger::set(unsigned p)
{
	progress = p;
	draw();
}

void ProgressLogger::increment()
{
	progress++;
	draw();
}

void ProgressLogger::draw()
{
	unsigned count = (2 * (LENGTH - 2) * progress + max) / (2 * max);

	stream << '\r';

	for (unsigned i = 0; i < count; ++i) {
		stream << "█";
	}

	for (unsigned i = 0; i < LENGTH - 2 - count; ++i) {
		stream << "░";
	}

	stream << std::flush;
}
