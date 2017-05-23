#pragma once
#include "QueryReporter.hpp"
#include "RunTimeReporter.hpp"

namespace Bench
{
	/**
	 * Starts the perf tool just before running through the query set
	 * repeatedly, clearing the cache between each run.
	 */
	class PerfReporter : public QueryReporter, RunTimeReporter
	{
		public:
			PerfReporter(
					const std::string& path,
					const std::string& event,
					unsigned burn = 0
				);

			virtual void run(
					const SpatialIndex& index,
					std::ostream& logStream
				);

		private:
			std::string event;
			unsigned burn;
	};
}
