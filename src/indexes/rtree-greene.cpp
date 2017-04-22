#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/GreeneRtree.hpp"
#include "rtree/DefaultNode.hpp"

using namespace Rtree;

SpatialIndex * create(const Box&, unsigned long long)
{
	return new GreeneRtree<DefaultNode<D, M>, m>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
