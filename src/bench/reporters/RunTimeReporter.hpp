#pragma once
#include <map>
#include <chrono>
#include <vector>

namespace Bench
{

/**
 * Reports the run time of each query.
 */
class RunTimeReporter
{
	public:
		using clock = std::chrono::steady_clock;
		using period = std::chrono::microseconds;

		/**
		 * Default constructor
		 *
		 * Checks that the system clocks are capable of delivering the requested
		 * resolution.
		 */
		RunTimeReporter();

	protected:
		/**
		 * Used to determine how the cache should be cleared.
		 */
		static const unsigned CACHE_SIZE = 4096; // kilobytes
		static const unsigned CACHE_LINE_SIZE = 64; // bytes

		/**
		 * "Clears" the cache by writing data to a large array.
		 * To save time, only one value in each cache line is written.
		 */
		void clearCache();
};

}
