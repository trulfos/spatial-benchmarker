#include "interface.hpp"
#include "configuration.hpp"
#include "rtree/QuadraticRtree.hpp"
#include "rtree/TransformingRtree.hpp"
#include "rtree/PointerArrayNode.hpp"

using namespace Rtree;

SpatialIndex * create(const Box&, unsigned long long)
{
	return new TransformingRtree<
			QuadraticRtree<D, M, m>,
			PointerArrayNode<D, M>
		>();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
