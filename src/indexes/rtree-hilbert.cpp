#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/HilbertRtree.hpp"
#include "rtree/HilbertEntryPlugin.hpp"

using namespace Rtree;

SpatialIndex * create(const Box& bounds, unsigned long long)
{
	return new HilbertRtree<
			Node<HilbertEntryPlugin>,
			s
		>(bounds);
}

void destroy(SpatialIndex * index)
{
	delete index;
}
