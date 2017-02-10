#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/HilbertRtree.hpp"

SpatialIndex * create()
{
	return new Rtree::HilbertRtree<D, M, s>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
