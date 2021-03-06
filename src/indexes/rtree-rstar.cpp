#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/RRStarTree.hpp"
#include "rtree/CapturingEntryPlugin.hpp"

using namespace Rtree;

SpatialIndex * create(const Box&, unsigned long long)
{
	return new RRStarTree<
			Node<CapturingEntryPlugin>,
			m
		>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
