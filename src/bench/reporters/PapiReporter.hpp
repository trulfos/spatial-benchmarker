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

			using QueryReporter::QueryReporter;

			void run(
					const SpatialIndex& index,
					std::ostream& logStream
				) override;

		private:
			static constexpr unsigned RUNS = 10;

	};

}
