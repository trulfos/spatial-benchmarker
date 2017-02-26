#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/HilbertRtree.hpp"

SpatialIndex * create(unsigned, unsigned long long)
{
	return new Rtree::HilbertRtree<D, M, s>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
