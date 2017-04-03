#pragma once
#include "MetricReporter.hpp"

namespace Bench
{

/**
 * Reports on the structure of an index.
 */
class StructReporter : public MetricReporter<unsigned long long>
{
	public:

		void run(
				const SpatialIndex& index,
				std::ostream& logStream
			) override;

};

}
