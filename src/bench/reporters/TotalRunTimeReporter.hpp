#pragma once
#include "RunTimeReporter.hpp"
#include "QueryReporter.hpp"

namespace Bench
{

/**
 * This reporter loads all queries to memory and executes them all while
 * recording the time. To avoid caching problems, the queries are randomly
 * reordered several times and rerun, giving an average runtime per query.
 *
 * This saves time when evaluating the index.
 */
class TotalRunTimeReporter : public QueryReporter, private RunTimeReporter
{
	public:

		TotalRunTimeReporter(const std::string& queryPath, unsigned runs);

		void run(
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

	private:
		unsigned runs;

};

}
