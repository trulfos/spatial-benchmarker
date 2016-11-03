#include "Logger.hpp"

Logger::Logger(std::ostream& stream, const std::string& supertask)
	: stream(stream)
{
	stream << supertask
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
	// Are we creating a subtask?
	if (!done) {
		stream << "\n";
	}
	
	// Handle indentation
	for (unsigned i = 0; i < level; i++) {
		stream << '\t';
	}

	level++;

	// Print task
	stream << " - " << task << "..." << std::flush;

	done = false;
}


void Logger::end()
{
	if (!done) {
		stream << C::green("DONE") << '\n';
		done = true;
	}

	level--;
}

void Logger::endStart(const std::string& task)
{
	end();
	start(task);
}
