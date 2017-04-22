#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/GreeneRtree.hpp"

using namespace Rtree;

SpatialIndex * create(const Box&, unsigned long long)
{
	return new GreeneRtree<Node<>, m>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
