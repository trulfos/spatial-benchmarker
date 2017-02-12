#pragma once
#include "common/ResultSet.hpp"
#include "Reporter.hpp"
#include <vector>

/**
 * Reports the results of a run.
 */
class ResultsReporter : public Reporter
{
	public:
		void run(
				const std::string& name,
				Benchmark& benchmark,
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

		void generate(std::ostream& stream) const override;

	protected:
		ResultSet resultSet;
		std::string reference;
};
