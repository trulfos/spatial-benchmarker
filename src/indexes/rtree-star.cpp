#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/RStarTree.hpp"

SpatialIndex * create(unsigned, unsigned long long)
{
	if (p != 0) {
		return new Rtree::RStarTree<D, M, m, p>();
	} else {
		return new Rtree::RStarTree<D, M, m, M/3>();
	}
}

void destroy(SpatialIndex * index)
{
	delete index;
}
