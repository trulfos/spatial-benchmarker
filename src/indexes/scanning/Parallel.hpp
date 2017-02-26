#pragma once
#include "Scanning.hpp"

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
		Results rangeSearch(const Box& box) const;
		Results knnSearch(unsigned k, const Point& point) const;
};

}
