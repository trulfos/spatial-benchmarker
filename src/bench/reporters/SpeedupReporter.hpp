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
		 * @param runs Number of runs to measure average over
		 * @param reference Reference algorithm name
		 */
		SpeedupReporter(unsigned runs, std::string reference);

		void generate(std::ostream& stream) const;
	
	private:
		std::string reference;
};
