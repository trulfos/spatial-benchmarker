cmake_minimum_required(VERSION 3.2)

project("Spatial Indexing Framework")
include(ExternalProject)


# We need PAPI
ExternalProject_Add(Papi
	URL http://icl.utk.edu/projects/papi/downloads/papi-5.5.1.tar.gz
	URL_HASH SHA1=92dfb479fb025177e5eb56df76afc96cd11eb81f
	SOURCE_DIR papi
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND cd src && ./configure
	BUILD_COMMAND cd src && make
	INSTALL_COMMAND ""
)


add_library(papi STATIC IMPORTED)
set_property(TARGET papi PROPERTY IMPORTED_LOCATION
	${CMAKE_CURRENT_BINARY_DIR}/papi/src/libpapi.a)
add_dependencies(papi Papi)


# TCLAP for argument parsing
ExternalProject_Add(Tclap
	URL https://downloads.sourceforge.net/project/tclap/tclap-1.2.1.tar.gz
	URL_HASH SHA1=4f124216dd6e6936f5af6372d921a6c51563f8fd
	SOURCE_DIR tclap
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
)


include_directories(src ${CMAKE_CURRENT_BINARY_DIR}/papi/src ${CMAKE_CURRENT_BINARY_DIR}/tclap/include)
set(optimized_flags -O3 -flto -fuse-linker-plugin)
set(CMAKE_EXE_LINKER_FLAGS "-O3 -flto -fuse-linker-plugin")
set(CMAKE_MODULE_LINKER_FLAGS "-O3 -flto -fuse-linker-plugin")

add_compile_options(
	-Wall -g -std=c++11 -march=native -fvisibility=hidden
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-D_GLIBCXX_DEBUG)
else()
	add_definitions(-DNDEBUG)
	add_compile_options(${optimized_flags})
endif()

add_library(spatial OBJECT
	src/spatial/RangeQuery.cpp
	src/spatial/Box.cpp
	src/spatial/DataObject.cpp
	src/spatial/Query.cpp
	src/spatial/SpatialIndex.cpp
	src/spatial/KnnQuery.cpp
	src/spatial/Point.cpp
	src/spatial/Results.cpp
)

target_compile_options(spatial PRIVATE -fPIC)

# Main executable
add_executable(bench
	src/bench/main.cpp
	src/bench/ReporterArg.cpp
	src/bench/QueryInputIterator.cpp
	src/bench/Logger.cpp
	src/bench/DataObjectInputIterator.cpp
	src/bench/BoxInputIterator.cpp

	# Reporters!
	src/bench/reporters/CorrectnessReporter.cpp
	src/bench/reporters/ResultSet.cpp
	src/bench/reporters/AvgStatsReporter.cpp
	src/bench/reporters/TotalRunTimeReporter.cpp
	src/bench/reporters/StatsReporter.cpp
	src/bench/reporters/RunTimeReporter.cpp
	src/bench/reporters/StructReporter.cpp
	src/bench/reporters/Reporter.cpp
	src/bench/reporters/QueryRunTimeReporter.cpp
	src/bench/reporters/QueryReporter.cpp
	src/bench/reporters/FileHeader.cpp
	src/bench/reporters/ProgressLogger.cpp
	src/bench/reporters/ResultsReporter.cpp
	src/bench/reporters/PapiReporter.cpp

	$<TARGET_OBJECTS:spatial>
)

target_link_libraries(bench m dl papi)
add_dependencies(bench Tclap)

# Add tests
enable_testing()

add_executable(test_point
	$<TARGET_OBJECTS:spatial>
	src/spatial/Point.test.cpp
)

add_executable(test_box
	$<TARGET_OBJECTS:spatial>
	src/spatial/Box.test.cpp
)

add_executable(test_knnqueueentry
	src/indexes/rtree/KnnQueueEntry.test.cpp
)

add_executable(test_hilbertcurve
	$<TARGET_OBJECTS:spatial>
	src/indexes/rtree/HilbertCurve.test.cpp
)

add_executable(test_mbr
	$<TARGET_OBJECTS:spatial>
	src/indexes/rtree/Mbr.test.cpp
)

add_executable(test_weightingfunction
	$<TARGET_OBJECTS:spatial>
	src/indexes/rtree/WeightingFunction.test.cpp
)

add_executable(test_goalfunction
	$<TARGET_OBJECTS:spatial>
	src/indexes/rtree/GoalFunction.test.cpp
)

add_executable(test_splitset
	$<TARGET_OBJECTS:spatial>
	src/indexes/rtree/SplitSet.test.cpp
)

add_executable(test_checkcomp
	$<TARGET_OBJECTS:spatial>
	src/indexes/rtree/CheckComp.test.cpp
)

add_executable(test_referenceview
	src/indexes/rtree/ReferenceView.test.cpp
)

add_executable(test_combinationsview
	src/indexes/rtree/CombinationsView.test.cpp
)

add_executable(test_quadraticseeds
	$<TARGET_OBJECTS:spatial>
	src/indexes/rtree/QuadraticSeeds.test.cpp
)

add_executable(test_vectorizednode
	$<TARGET_OBJECTS:spatial>
	src/indexes/rtree/VectorizedNode.test.cpp
)

foreach(name
		point
		box
		knnqueueentry
		hilbertcurve
		mbr
		weightingfunction
		goalfunction
		combinationsview
		splitset
		checkcomp
		referenceview
		quadraticseeds
		vectorizednode
	)
	add_test(NAME ${name} COMMAND test_${name})
	target_link_libraries(test_${name} criterion)
	# TODO: This should also set appropriate compile options to avoid optimizing
	# away the tests.
endforeach()


# Add spatial indexes
set(D 2 CACHE STRING "Dimensionality of compiled indexes")
set(M 128 CACHE STRING "Node capacity for R-tree indexes")
set(m 64 CACHE STRING "Minimum node fill grade for R-tree indexes")
set(p 0 CACHE STRING "Number of nodes to reinsert for R*-tree")
set(s 2 CACHE STRING "Hilbert R-tree split strategy s:(s+1)")
set(N "DefaultNode" CACHE STRING "Node type to use for R-trees")

configure_file(
	src/indexes/configuration.hpp.in
	configuration.hpp
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# R-trees
foreach(name rtree rtree-hilbert rtree-star rtree-rstar rtree-greene)
	add_library(${name} MODULE EXCLUDE_FROM_ALL
		src/indexes/${name}.cpp
		$<TARGET_OBJECTS:spatial>
	)
endforeach()

# Scanning indexes
add_library(sequential MODULE EXCLUDE_FROM_ALL
	src/indexes/sequential.cpp
	src/indexes/scanning/Scanning.cpp
	src/indexes/scanning/Sequential.cpp
	$<TARGET_OBJECTS:spatial>
)

add_library(parallel MODULE EXCLUDE_FROM_ALL
	src/indexes/parallel.cpp
	src/indexes/scanning/Scanning.cpp
	src/indexes/scanning/Parallel.cpp
	$<TARGET_OBJECTS:spatial>
)

target_link_libraries(parallel -fopenmp)
target_compile_options(parallel PRIVATE -fopenmp)

# The rest (TM)
foreach(name naive vectorized)
	add_library(${name} MODULE EXCLUDE_FROM_ALL
		src/indexes/${name}.cpp
		src/indexes/${name}/SpatialIndex.cpp
		$<TARGET_OBJECTS:spatial>
	)
endforeach()

target_link_libraries(vectorized -fopenmp)
target_compile_options(vectorized PRIVATE -fopenmp)
