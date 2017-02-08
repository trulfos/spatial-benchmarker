#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/GreeneTree.hpp"

SpatialIndex * create()
{
	return new Rtree::GreeneRtree<D, M>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
