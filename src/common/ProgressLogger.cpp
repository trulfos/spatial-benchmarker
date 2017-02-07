#include "ProgressLogger.hpp"
#include <string>

ProgressLogger::ProgressLogger(std::ostream& stream, unsigned max)
	: stream(stream), max(max)
{
	draw();
}

ProgressLogger::~ProgressLogger()
{
	stream << std::endl;
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

	stream << "\r\033[1;2m["
			<< std::string(count, '=')
			<< std::string((LENGTH - 2) - count, ' ')
		<< "]\033[0m";

	if (progress == max) {
		stream << std::endl;
	} else {
		stream << std::flush;
	}
}
