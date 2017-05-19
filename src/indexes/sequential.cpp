#include "interface.hpp"
#include "scanning/Sequential.hpp"

SpatialIndex * create(const Box& box, unsigned long long size)
{
	return new Scanning::Sequential(box.getDimension(), size);
}

void destroy(SpatialIndex * index)
{
	delete index;
}
