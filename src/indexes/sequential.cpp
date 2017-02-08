#include "interface.hpp"
#include "sequential/SpatialIndex.hpp"

SpatialIndex * create()
{
	return new Sequential::SpatialIndex();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
