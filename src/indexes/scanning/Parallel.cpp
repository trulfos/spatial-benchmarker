#include "Parallel.hpp"

namespace Scanning
{

void Parallel::rangeSearch(Results& results, const Box& box) const
{
	const Point& bottom = box.getPoints().first;
	const Point& top = box.getPoints().second;

	// Scan through data
#	pragma omp parallel for schedule(static)
	for (unsigned i = 0; i < nObjects; i++) {
		bool inside = true;

		for (unsigned j = 0; j < dimension; j++) {
			const unsigned k = 2 * (dimension * i + j);
			inside &= top[j] >= positions[k] && positions[k + 1] >= bottom[j];
		}

		if (inside) {
			//TODO: Is this a bottleneck. Alternative: Combine sets after loop
#			pragma omp critical
			results.push_back(ids[i]);
		}
	}
};


void Parallel::knnSearch(Results&, unsigned k, const Point& point) const
{
	throw std::logic_error("Not implemented");
};

}
