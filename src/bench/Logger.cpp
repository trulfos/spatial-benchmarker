#include "Logger.hpp"
#include "Color.hpp"

namespace Bench
{

Logger::Logger(std::ostream& stream, const std::string& supertask)
	: stream(stream)
{
	stream << C::bold(supertask)
		<< "\n----------------------------------------" << std::endl;
}

Logger::~Logger()
{
	while(level)
	{
		end();
	}
}

void Logger::start(const std::string& task)
{
	// Handle indentation
	for (unsigned i = 0; i < level; i++) {
		stream << '\t';
	}

	level++;

	// Print task
	stream << " - " << task << std::endl;
}


void Logger::end()
{
	if (level == 0) {
		throw std::logic_error("Trying to end a task that never started");
	}

	level--;
}

void Logger::endStart(const std::string& task)
{
	end();
	start(task);
}

}
