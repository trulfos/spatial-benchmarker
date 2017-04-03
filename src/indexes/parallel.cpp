#include "interface.hpp"
#include "scanning/Parallel.hpp"

SpatialIndex * create(const Box& bounds, unsigned long long size)
{
	return new Scanning::Parallel(bounds.getDimension(), size);
}

void destroy(SpatialIndex * index)
{
	delete index;
}
