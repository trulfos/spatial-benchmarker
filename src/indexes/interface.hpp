#pragma once
#include "bench/SpatialIndex.hpp"

/**
 * Create a new index and return a pointer to it.
 *
 * @param dimension Dimensionality of data set
 * @param size Number of elements to allocate space for
 */
extern "C"
__attribute__ ((visibility ("default")))
SpatialIndex * create(unsigned dimension, unsigned long long size);

/**
 * Destroy a previously returned index, freeing the associated resources.
 */
extern "C"
__attribute__ ((visibility ("default")))
void destroy(SpatialIndex * index);
