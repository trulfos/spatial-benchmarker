#include "Sequential.hpp"
#include <algorithm>
#include <functional>

namespace Scanning
{

Results Sequential::rangeSearch(const Box& box) const
{
	Results results;
	const Point& bottom = box.getPoints().first;
	const Point& top = box.getPoints().second;

	// Scan through data
	for (unsigned i = 0; i < nObjects; i++) {
		bool intersects = true;

		for (unsigned j = 0; j < dimension; j++) {
			const unsigned k = 2 * (dimension * i + j);
			intersects &= (
					top[j] >= positions[k] && positions[k + 1] >= bottom[j]
				);
		}

		if (intersects) {
			results.push_back(ids[i]);
		}
	}

	return results;
};


Results Sequential::knnSearch(unsigned k, const Point& point) const
{
	throw std::logic_error("KNN search not implemented");
};

}
