#include "interface.hpp"
#include "vectorized/SpatialIndex.hpp"

SpatialIndex * create(const Box& bounds, unsigned long long size)
{
	return new Vectorized::SpatialIndex(bounds.getDimension(), size);
}

void destroy(SpatialIndex * index)
{
	delete index;
}
