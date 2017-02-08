#pragma once
#include "bench/SpatialIndex.hpp"

extern "C"
__attribute__ ((visibility ("default")))
SpatialIndex * create();

extern "C"
__attribute__ ((visibility ("default")))
void destroy(SpatialIndex * index);
