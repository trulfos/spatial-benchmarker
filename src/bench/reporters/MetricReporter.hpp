#pragma once
#include "Reporter.hpp"

/**
 * Abstract class for a metric reporter.
 *
 * A metric reporter records one or several metrics per query and spits all of
 * them out in a giant list which can easily be used for inserting entries in a
 * relational database.
 *
 * Sub classes should call the `addEntry` method to add rows in the output.
 */
class MetricReporter : public Reporter
{
	public:
		/**
		 * Output this report to the given stream.
		 */
		virtual void generate(std::ostream& stream) const;


	protected:

		/**
		 * Add a new entry to the output table.
		 *
		 * The output will be formatted automatically by this class when
		 * `generate` is called.
		 *
		 * @param query Query the metric is valid for
		 * @param metric Metric name
		 * @param value Value measured for the given query-metric pair
		 */
		void addEntry(
				const Query& query,
				const std::string& metric,
				const std::string& value
			);


	private:

		struct ResultEntry
		{
			std::string query;
			std::string metric;
			std::string value;
		};

		std::vector<ResultEntry> results;
};

std::ostream& operator<<(
		std::ostream& stream, const std::shared_ptr<Reporter>& reporter
	);
