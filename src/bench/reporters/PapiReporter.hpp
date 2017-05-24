#pragma once
#include "QueryReporter.hpp"
#include "RunTimeReporter.hpp"

namespace Bench
{

	/**
	 * Collects performance metrics through PAPI and reports the results.
	 */
	class PapiReporter : public QueryReporter, private RunTimeReporter
	{
		public:

			PapiReporter(
					const std::string& queryPath,
					unsigned runs = 1,
					unsigned burns = 0,
					const std::vector<std::string>& events = {"PAPI_TOT_INS"}
				);

			void run(
					const SpatialIndex& index,
					std::ostream& logStream
				) override;

		private:

			/**
			 * Number of times to reorder the queries. The results are averages
			 * of the numbers for each of these runs.
			 */
			static constexpr unsigned REORDER_RUNS = 10;

			/**
			 * Number of times to repeat the reodering and running of queries.
			 */
			unsigned runs;

			/**
			 * Number of runs to burn before starting measurements.
			 */
			unsigned burns;

			/**
			 * Events to record
			 */
			std::vector<std::string> events;

	};

}
