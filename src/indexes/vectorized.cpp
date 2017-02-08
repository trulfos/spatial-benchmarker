#include "interface.hpp"
#include "vectorized/SpatialIndex.hpp"

SpatialIndex * create()
{
	return new Vectorized::SpatialIndex();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
