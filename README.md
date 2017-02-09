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

### Bencmarker
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

### Indexes

You should normally specify the options for an index when compiling it. This
includes the dimnsionality. This will build the hilbert R-tree with a node size
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

### Benchmark folder structure
A benchmark consists of a folder with the following files:

#### dimension
This file contains the dimensionality of the data as text. E.g.
```
9
```
for a 9-dimensional benchmark.


#### data
Data set as a sequence of boxes. Each box is given by the lower and upper
coordinate in each dimension (as binary doubles).

#### queries
Same format as data file. These are assumed to be range queries.

#### results
The correct results of the queries above queries. Each line starts with the
number of results, followed by the included boxes from the K

### Generating results
The benchmarker expects there to be valid results in the same folder as the data
and queries. Correct results are best generated using the naive index.

To turn off the checking of results, use the `--no-check` option and
generate a results report using `--report results`:
```bash
bin/bench yourbenchmark/ -i naive -r results -n > yourbenchmark/results
```
