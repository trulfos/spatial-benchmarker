#include "interface.hpp"
#include "scanning/Sequential.hpp"

SpatialIndex * create(unsigned dimension, unsigned long long size)
{
	return new Scanning::Sequential(dimension, size);
}

void destroy(SpatialIndex * index)
{
	delete index;
}
