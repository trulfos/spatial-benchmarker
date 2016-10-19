#pragma once
#include <chrono>
#include <functional>

/**
 * Simple class for measuring the time required to execute a function.
 */
class Timer
{
	public:

		using clock = std::chrono::steady_clock;
		using period = std::chrono::microseconds;

		Timer();

		/**
		 * Measure the wall clock run time of the given task.
		 *
		 * The result is an integer number of periods.
		 */
		unsigned long timeTask(std::function<void()> task);
};
