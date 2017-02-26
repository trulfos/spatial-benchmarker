#include "interface.hpp"
#include "scanning/Parallel.hpp"

SpatialIndex * create(unsigned dimension, unsigned long long size)
{
	return new Scanning::Parallel(dimension, size);
}

void destroy(SpatialIndex * index)
{
	delete index;
}
