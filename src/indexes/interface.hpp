#pragma once
#include "spatial/SpatialIndex.hpp"

using namespace Spatial;

/**
 * Create a new index and return a pointer to it.
 *
 * @param bounds Bounds of the data domain (implicitly also dimension)
 * @param size Number of elements to allocate space for
 */
extern "C"
__attribute__ ((visibility ("default")))
SpatialIndex * create(const Box& bounds, unsigned long long size);

/**
 * Destroy a previously returned index, freeing the associated resources.
 */
extern "C"
__attribute__ ((visibility ("default")))
void destroy(SpatialIndex * index);
