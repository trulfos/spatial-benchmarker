#include "interface.hpp"
#include "naive/SpatialIndex.hpp"

SpatialIndex * create(const Box&, unsigned long long)
{
	return new Naive::SpatialIndex();
}

void destroy(SpatialIndex * index)
{
	delete index;
}
