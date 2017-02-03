# Spatial index test framework

## Dependencies
Dependencies (with ubuntu package name)

- Templatized C++ Command Line Parser Manual (libtclap-dev)
- OpenMP (libgomp1 for gcc)

For running unit tests

- Criterion


## Building & running
Build the benchmarker with
```bash
make bin/bench
```

For a overview of command line arguments, run
```bash
bin/bench --help
```


Build `mkdata` and `mkqueries` using
```bash
make bin/mkdata
make bin/mkqueries
```
Use the `--help` option for printing command line options for these two
commands.

Run all unit tests
```bash
make check
```

You can also make and run a specific unit test
```bash
make test/bench/rtree/Mbr
./test/bench/rtree/Mbr
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
