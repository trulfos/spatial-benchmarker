#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/RRStarTree.hpp"

SpatialIndex * create(unsigned, unsigned long long)
{
	return new Rtree::RRStarTree<D, M, m>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
