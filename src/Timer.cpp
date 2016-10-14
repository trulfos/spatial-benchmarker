#include "Timer.hpp"
#include <stdexcept>

Timer::Timer()
{
	if (clock::period::num * period::period::den > clock::period::den * period::period::num) {
		throw std::runtime_error(
			"The clock resolution is too low for the selected period"
		);
	}
}


unsigned long Timer::timeTask(std::function<void()> task) {

	auto startTime = clock::now();
	task();
	auto endTime = clock::now();

	return std::chrono::duration_cast<std::chrono::microseconds>(
			endTime - startTime
		).count();
}
