#pragma once
#include "MetricReporter.hpp"

/**
 * Checks the correctness of the results given to this reporter and spits out an
 * error if any of the results are wrong.
 */
class CorrectnessReporter : public Reporter
{
	public:

		void run(
				const std::string& name,
				Benchmark& benchmark,
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

		void generate(std::ostream& stream) const override;

	private:
		std::vector<bool> correct;

};
