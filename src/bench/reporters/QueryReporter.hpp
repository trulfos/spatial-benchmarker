#pragma once
#include "Reporter.hpp"
#include "bench/LazyQuerySet.hpp"

/**
 * This (base) reporter allows loading queries from file.
 */
class QueryReporter : public Reporter
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
