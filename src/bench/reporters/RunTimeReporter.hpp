#pragma once
#include "MetricReporter.hpp"
#include <map>
#include <chrono>
#include <vector>

/**
 * Reports the run time of each query.
 */
class RunTimeReporter : public MetricReporter
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


		Results run(
				const std::string& name,
				const Query& query,
				const SpatialIndex& index
			);

	protected:
		/**
		 * Used to determine how the cache should be cleared.
		 */
		const unsigned CACHE_SIZE = 4096; // kilobytes
		const unsigned CACHE_LINE_SIZE = 64; // bytes

		/**
		 * The run time will be measured within these constraints (in order of
		 * priority).
		 */
		const unsigned MAX_RUNS = 25;
		const unsigned long MIN_TOTAL_TIME = 5 * 1e6; // µs

		/**
		 * "Clears" the cache by writing data to a large array.
		 * To save time, only one value in each cache line is written.
		 */
		void clearCache();
};
