#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/GreeneRtree.hpp"

SpatialIndex * create(const Box&, unsigned long long)
{
	return new Rtree::GreeneRtree<D, M, m>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
