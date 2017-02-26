#pragma once
#include <ostream>

/**
 * Prints progress to the console.
 */
class ProgressLogger
{
	public:

		/**
		 * Create a new logger, logging to the given stream, with the given max.
		 *
		 * @param stream Stream to log to
		 * @param max Maximum progress value
		 */
		ProgressLogger(std::ostream& stream, unsigned long long max = 100);

		/**
		 * Set progress to the given value.
		 *
		 * @param progress Progress relative to max
		 */
		void set(unsigned long long progress);

		/**
		 * Increment progress by one.
		 */
		void increment();

	private:
		const unsigned LENGTH = 80;
		unsigned long long progress = 0;
		std::ostream& stream;
		unsigned max;
};
