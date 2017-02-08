#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/RStarTree.hpp"

SpatialIndex * create()
{
	return new Rtree::RStarTree<D, M>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
