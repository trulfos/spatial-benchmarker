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
		void rangeSearch(Results& results, const Box& box) const override;
		void knnSearch(Results& results, unsigned k, const Point& point) const override;
};

}
