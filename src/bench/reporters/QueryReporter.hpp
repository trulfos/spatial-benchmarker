#pragma once
#include "MetricReporter.hpp"
#include "bench/LazyQuerySet.hpp"

namespace Bench
{

/**
 * This (base) reporter allows loading queries from file.
 */
class QueryReporter : public MetricReporter<double>
{
	public:

		/**
		 * Construct a new reporter with the given query file.
		 */
		QueryReporter(const std::string& queryPath);

	protected:

		/**
		 * Get a lazy query set.
		 */
		LazyQuerySet getQuerySet() const;


	private:

		std::string path;
};

}
