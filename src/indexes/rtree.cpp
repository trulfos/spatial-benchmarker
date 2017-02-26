#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/QuadraticTree.hpp"

SpatialIndex * create(unsigned, unsigned long long)
{
	return new Rtree::QuadraticRtree<D, M, m>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
