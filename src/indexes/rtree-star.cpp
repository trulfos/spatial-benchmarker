#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/RStarTree.hpp"
#include "rtree/DefaultNode.hpp"

using namespace Rtree;

SpatialIndex * create(const Box&, unsigned long long)
{
	if (p != 0) {
		return new RStarTree<DefaultNode<D, M>, m, p>();
	} else {
		return new RStarTree<DefaultNode<D, M>, m, M/3>();
	}
}

void destroy(SpatialIndex * index)
{
	delete index;
}
