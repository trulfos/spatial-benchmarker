#pragma once
#include "common/Query.hpp"
#include "common/Results.hpp"
#include "bench/SpatialIndex.hpp"
#include <functional>
#include <ostream>
#include <string>

/**
 * Abstract base class for reporter.
 */
class Reporter
{
	public:
		/**
		 * Run the given task and record whatever this reporter is going to
		 * report.
		 *
		 * Several runs can use the same name. This results in the results being
		 * grouped by name.
		 *
		 * @param name Name to use in report
		 * @param task Task to run
		 */
		virtual Results run(
				const std::string& name,
				const Query& query,
				const SpatialIndex& index
			) = 0;

		/**
		 * Output this report to the given stream.
		 */
		virtual void generate(std::ostream& stream) const = 0;
};

std::ostream& operator<<(
		std::ostream& stream, const std::shared_ptr<Reporter>& reporter
	);
