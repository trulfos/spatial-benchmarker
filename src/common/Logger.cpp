#include "Logger.hpp"
#include "common/Color.hpp"

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
	level--;
}

void Logger::endStart(const std::string& task)
{
	end();
	start(task);
}
