#pragma once
#include <ostream>
#include <string>
#include "Color.hpp"

/**
 * Simple class to log progress while doing calculations.
 */
class Logger
{
	public:
		/**
		 * Create a new logger, logging to the given stream.
		 *
		 * @param stream Destination stream for log
		 * @param supertask Main task performed during logging
		 */
		Logger(std::ostream& stream, const std::string& supertask);

		~Logger();

		/**
		 * Start a new task.
		 * This also marks the previous task as completed.
		 *
		 * @param description Human readable task description
		 * @param super True if this is a super task
		 */
		void start(const std::string& task);


		/**
		 * Shortcut for a call to end, then start.
		 */
		void endStart(const std::string& task);

		/**
		 * End the current (super)task.
		 */
		void end();

	private:
		unsigned level = 0;
		std::ostream& stream;
};
