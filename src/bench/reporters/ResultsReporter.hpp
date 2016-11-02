#pragma once
#include "../../common/ResultSet.hpp"
#include "Reporter.hpp"
#include <vector>

/**
 * Reports the results of a run.
 */
class ResultsReporter : public Reporter
{
	public:
		Results run(
				const std::string& name,
				const Query& query,
				const SpatialIndex& index
			);

		void generate(std::ostream& stream) const;

	protected:
		ResultSet resultSet;
		std::string reference;
};
