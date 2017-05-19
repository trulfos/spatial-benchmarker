#pragma once
#include "Scanning.hpp"

using namespace Spatial;

namespace Scanning
{

/**
 * Parallel scan of all objects using OpenMP.
 */
class Parallel : public Scanning
{
	public:
		using Scanning::Scanning;

	protected:
		void rangeSearch(Results& results, const Box& box) const;
		void knnSearch(Results& results, unsigned k, const Point& point) const;
};

}
