#pragma once
#include "ResultSet.hpp"
#include "QueryReporter.hpp"

namespace Bench
{

/**
 * Checks the correctness of the results given to this reporter and spits out an
 * error if any of the results are wrong.
 */
class CorrectnessReporter : public QueryReporter
{
	public:

		/**
		 * Construct a new correctness reporter using the given query and result
		 * file.
		 */
		CorrectnessReporter(
				const std::string& queryPath,
				const std::string& resultsPath
			);

		void run(
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

		void generate(std::ostream& stream) const override;

	private:
		std::vector<unsigned> incorrect;
		std::string resultsPath;

		ResultSet getResults();

};

}
