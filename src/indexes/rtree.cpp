#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/QuadraticTree.hpp"

SpatialIndex * create()
{
	return new Rtree::QuadraticRtree<D, M>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
