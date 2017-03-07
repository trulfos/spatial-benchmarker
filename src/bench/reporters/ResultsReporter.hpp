#pragma once
#include "ResultSet.hpp"
#include "QueryReporter.hpp"
#include <vector>

/**
 * Reports the results of a run.
 */
class ResultsReporter : public QueryReporter
{
	public:
		using QueryReporter::QueryReporter;

		void run(
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

		void generate(std::ostream& stream) const override;

	protected:
		ResultSet resultSet;
		std::string reference;
};
