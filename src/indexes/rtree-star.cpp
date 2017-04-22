#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/RStarTree.hpp"

using namespace Rtree;

SpatialIndex * create(const Box&, unsigned long long)
{
	if (p != 0) {
		return new RStarTree<Node<>, m, p>();
	} else {
		return new RStarTree<Node<>, m, M/3>();
	}
}

void destroy(SpatialIndex * index)
{
	delete index;
}
