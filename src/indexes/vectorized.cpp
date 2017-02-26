#include "interface.hpp"
#include "vectorized/SpatialIndex.hpp"

SpatialIndex * create(unsigned dimension, unsigned long long size)
{
	return new Vectorized::SpatialIndex(dimension, size);
}

void destroy(SpatialIndex * index)
{
	delete index;
}
