cmake_minimum_required(VERSION 3.7)

project("Spatial Indexing Framework")

include_directories(src)

set(optimized_flags -fvisibility=hidden -O3 -flto -fuse-linker-plugin -DNDEBUG)

add_compile_options(
	-Wall -g -std=c++11 -fopenmp -march=native -fvisibility=hidden
)

link_libraries(${name} -fopenmp optimized ${optimized_flags})

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
	add_definitions(-D_GLIBCXX_DEBUG)
else()
	add_definitions(-DNDEBUG)
	add_compile_options(${optimized_flags})
endif()

# Main executable
add_executable(bench
	src/bench/main.cpp

	src/bench/reporters/StatsReporter.cpp
	src/bench/reporters/RunTimeReporter.cpp
	src/bench/reporters/Reporter.cpp
	src/bench/reporters/MetricReporter.cpp
	src/bench/reporters/ResultsReporter.cpp
	src/bench/ReporterArg.cpp
	src/bench/SpatialIndex.cpp
	src/bench/QueryInputIterator.cpp
	src/bench/Benchmark.cpp
	src/bench/DataObjectInputIterator.cpp
	src/bench/BoxInputIterator.cpp

	src/common/Box.cpp
	src/common/DataObject.cpp
	src/common/FileHeader.cpp
	src/common/KnnQuery.cpp
	src/common/Logger.cpp
	src/common/Point.cpp
	src/common/ProgressLogger.cpp
	src/common/Query.cpp
	src/common/RangeQuery.cpp
	src/common/Results.cpp
	src/common/ResultSet.cpp
)

target_link_libraries(bench m dl)

# Add tests
enable_testing()

add_executable(test_point
	src/common/Point.cpp
	src/common/Point.test.cpp
)

add_executable(test_box
	src/common/Point.cpp
	src/common/Box.cpp
	src/common/Box.test.cpp
)

add_executable(test_knnqueueentry
	src/indexes/rtree/KnnQueueEntry.test.cpp
)

add_executable(test_hilbertcurve
	src/indexes/rtree/HilbertCurve.test.cpp
)

add_executable(test_mbr
	src/common/Point.cpp
	src/common/Box.cpp
	src/indexes/rtree/Mbr.test.cpp
)

foreach(name point box knnqueueentry hilbertcurve mbr)
	add_test(NAME ${name} COMMAND test_${name})
	target_link_libraries(test_${name} criterion)
endforeach()

# Framework files for indexes
add_library(common STATIC
	src/bench/BoxInputIterator.cpp
	src/bench/DataObjectInputIterator.cpp
	src/bench/SpatialIndex.cpp

	src/common/Point.cpp
	src/common/Box.cpp
	src/common/DataObject.cpp
	src/common/Query.cpp
	src/common/KnnQuery.cpp
	src/common/RangeQuery.cpp
)


# Add spatial indexes
set(D 2 CACHE STRING "Dimensionality of compiled indexes")
set(M 128 CACHE STRING "Node capacity for R-tree indexes")
set(m 64 CACHE STRING "Minimum node fill grade for R-tree indexes")
set(p 42 CACHE STRING "Number of nodes to reinsert for R*-tree")

configure_file(
	src/indexes/configuration.hpp.in
	configuration.hpp
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# R-trees
foreach(name rtree rtree-hilbert rtree-star rtree-rstar rtree-greene)
	add_library(${name} MODULE EXCLUDE_FROM_ALL
		src/indexes/${name}.cpp
	)

	target_link_libraries(${name} common)
endforeach()

# The rest
foreach(name naive sequential vectorized parallel)
	add_library(${name} MODULE EXCLUDE_FROM_ALL
		src/indexes/${name}.cpp
		src/indexes/${name}/SpatialIndex.cpp
	)

	target_link_libraries(${name} common)
endforeach()
