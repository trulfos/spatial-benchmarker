#include "interface.hpp"
#include "parallel/SpatialIndex.hpp"

SpatialIndex * create()
{
	return new Parallel::SpatialIndex();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
