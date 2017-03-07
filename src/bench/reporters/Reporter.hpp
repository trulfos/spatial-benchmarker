#pragma once
#include "common/SpatialIndex.hpp"
#include <ostream>

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
		 * @param index Spatial index to benchmark
		 * @param logStream Destination stream for log output
		 */
		virtual void run(
				const SpatialIndex& index,
				std::ostream& logStream
			) = 0;

		/**
		 * Output this report to the given stream.
		 */
		virtual void generate(std::ostream& stream) const = 0;
};

std::ostream& operator<<(
		std::ostream& stream, const std::shared_ptr<Reporter>& reporter
	);
