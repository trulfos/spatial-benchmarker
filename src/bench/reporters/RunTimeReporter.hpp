#pragma once
#include "Reporter.hpp"
#include <map>
#include <chrono>
#include <vector>

/**
 * Reports the run time of each query.
 */
class RunTimeReporter : public Reporter
{
	public:
		using clock = std::chrono::steady_clock;
		using period = std::chrono::microseconds;

		/**
		 * Create a new run time reporter measuring the run time as the minimum
		 * over the given number of runs.
		 *
		 * @param runs Number of runs to measure average over
		 */
		RunTimeReporter(unsigned runs);

		Results run(
				const std::string& name,
				const Query& query,
				const SpatialIndex& index
			);

		void generate(std::ostream& stream) const;

	protected:
		const unsigned CACHE_SIZE = 4096; // kilobytes
		const unsigned CACHE_LINE_SIZE = 64; // bytes

		std::map<std::string, std::vector<unsigned long>> timeseries;
		std::vector<std::string> queries;
		unsigned runs;

		/**
		 * "Clears" the cache by writing data to a large array.
		 * To save time, only one value in each cache line is written.
		 */
		void clearCache();
};
