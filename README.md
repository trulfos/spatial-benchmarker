# Spatial index benchmarking framework

The spatial index benchmarking framework contains code needed to benchmark
spatial indexes and a small set of indexes.

Each index is compiled into a dynamic library that is loaded by the benchmarker
at run time. The benchmarker loads the data, query and result set from a
benchmark folder. Depending on which reporter is selected, the data is indexed
and one or several metrics recorded and outputted.

Configuration parameters for the indexes (such as dimension) is set at compile
time, and to simplify the process of compiling the index and running the
benchmarker, some python scripts are included in the `scripts` directory. In
addition to simplifying compilation, these scripts also allow results to be
stored in an SQLite file for later retrieval.


## Dependencies
Dependencies (with ubuntu package name)

- Templatized C++ Command Line Parser Manual (libtclap-dev)
- OpenMP (libgomp1 for gcc)
- CMake
- Python 3 (for running scripts)

For running unit tests

- Criterion


## Building & running

CMake is used to generate build files.

### Benchmarker
An out of source build can be done by running
```bash
mkdir build
cd build
cmake ..
make bench
```

For a overview of command line arguments, run
```bash
bench --help
```

### Reporters

The benchmarker supports several reporters and runs the reporters given by
reporter definitions on the command line.

A reporter definition consists of the reporter name and (possibly) extra
arguments for the reporter, separated  by a `:` character:
```
<reporter definition> := <reporter name>[<argument>(':'<argument>)*]
```

For example, to run the statisitcs reporter for the `querysets/queryset1`
queryset, the reporter definition
```
stats:queryset/queryset1
```
can be used.

### Indexes

You should normally specify the options for an index when compiling it. This
includes the dimensionality. This will build the hilbert R-tree with a node size
of 128 for data sets of dimension 3.
```bash
cmake -DD=3 -DM=128 ..
make rtree-hilbert
```

The `scripts/benchmark` script automatically compiles the benchmark for the
correct dimension. For example, the following compiles the hilbert R-tree and
runs the `abs02` benchmark in the benchmark folder with a node size of 128.
```bash
./scripts/benchmark rtree-hilbert benchmarks/abs02/ -DM=128
```

The results of the above are recorded in `results` (an sqlite database) and can
be retrieved either manually using the `sqlite3` command or by running
`scripts/plot`.


### Unit tests

Run all unit tests
```bash
make
make test
```

You can also make and run a specific unit test
```bash
make test_mbr
./test_mbr
```

## Benchmarks

Data and query sets by Beckmann and Seeger can be downloaded from
[Becmann and Seeger's site](http://www.mathematik.uni-marburg.de/~seeger/rrstar/).
