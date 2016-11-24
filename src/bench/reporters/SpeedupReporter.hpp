#pragma once
#include "RunTimeReporter.hpp"

/**
 * Reports the speed up in run time relative to one of the algorithms.
 */
class SpeedupReporter : public RunTimeReporter
{
	public:

		/**
		 * Create a new speedup reporter using the given algorithm as baseline.
		 * The runtime is measured as the minimum run time over the given number
		 * of runs.
		 *
		 * @param reference Reference algorithm name
		 */
		SpeedupReporter(std::string reference);

		void generate(std::ostream& stream) const;
	
	private:
		std::string reference;
};
