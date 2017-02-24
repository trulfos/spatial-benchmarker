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
template<typename T = double>
class MetricReporter : public Reporter
{
	public:
		using value_type = T;

		/**
		 * Output this report to the given stream.
		 */
		virtual void generate(std::ostream& stream) const override;


	protected:

		/**
		 * Add a new entry to the output table.
		 *
		 * The output will be formatted automatically by this class when
		 * `generate` is called.
		 *
		 * @param index Index of result
		 * @param metric Metric name
		 * @param value Value measured for the given query-metric pair
		 */
		void addEntry(
				const std::string& metric,
				value_type value
			);


	private:

		struct ResultEntry
		{
			std::string name;
			value_type value;
		};

		std::vector<ResultEntry> results;
};

std::ostream& operator<<(
		std::ostream& stream, const std::shared_ptr<Reporter>& reporter
	);


/**
 * Implementations
 */


template<typename T>
void MetricReporter<T>::addEntry(
		const std::string& name,
		value_type value
	)
{
	results.emplace_back(
			ResultEntry {name, value}
		);
}


template<typename T>
void MetricReporter<T>::generate(std::ostream& stream) const
{
	// Print header
	stream << "name\tvalue\n";

	// Print data
	for (const auto& r : results) {
		stream << r.name << '\t'
			<< r.value << '\n';
	}

	stream << std::flush;
}
