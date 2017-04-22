#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/QuadraticRtree.hpp"
#include "rtree/DefaultNode.hpp"

SpatialIndex * create(const Box&, unsigned long long)
{
	return new Rtree::QuadraticRtree<Rtree::DefaultNode<D, M>, m>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
