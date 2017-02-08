#include "interface.hpp"
#include "naive/SpatialIndex.hpp"

SpatialIndex * create()
{
	return new Naive::SpatialIndex();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
