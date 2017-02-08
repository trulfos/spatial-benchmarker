#pragma once
#include "bench/SpatialIndex.hpp"

/**
 * Create a new index and return a pointer to it.
 */
extern "C"
__attribute__ ((visibility ("default")))
SpatialIndex * create();

/**
 * Destroy a previously returned index, freeing the associated resources.
 */
extern "C"
__attribute__ ((visibility ("default")))
void destroy(SpatialIndex * index);
