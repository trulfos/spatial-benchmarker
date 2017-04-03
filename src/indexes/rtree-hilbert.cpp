#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/HilbertRtree.hpp"

SpatialIndex * create(const Box& bounds, unsigned long long)
{
	return new Rtree::HilbertRtree<D, M, s>(bounds);
}

void destroy(SpatialIndex * index)
{
	delete index;
}
