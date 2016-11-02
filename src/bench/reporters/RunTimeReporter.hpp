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

		RunTimeReporter();

		Results run(
				const std::string& name,
				const Query& query,
				const SpatialIndex& index
			);

		void generate(std::ostream& stream) const;

	protected:
		std::map<std::string, std::vector<unsigned long>> timeseries;
		std::vector<std::string> queries;
};
