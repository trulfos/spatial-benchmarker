#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/QuadraticRtree.hpp"

SpatialIndex * create(const Box&, unsigned long long)
{
	return new Rtree::QuadraticRtree<D, M, m>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
