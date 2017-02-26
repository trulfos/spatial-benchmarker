#pragma once
#include "Scanning.hpp"

namespace Scanning
{

/**
 * Sequential scan of all objects.
 */
class Sequential : public Scanning
{

	public:
		using Scanning::Scanning;

	protected:
		Results rangeSearch(const Box& box) const override;
		Results knnSearch(unsigned k, const Point& point) const override;
};

}
