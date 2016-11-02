# Framework for testing spatial indexes

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

Run benchmarker on all benchmarks
```bash
make run
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
make test_all
```

You can also make and run a specific unit test
```bash
make test/bench/rtree/Mbr
./test/bench/rtree/Mbr
```

## Generating test data
Test data can be generated. Run `bin/mkdata` to generate a test data set, then
use `bin/mkqueries` to generate a test query set.

Running `bin/bench` with the above queries and data will fail due to missing
results. Turn off the checking of results with the `--no-check` option and
generate a results report using `--report results`, preferrably using the naive
index:
```bash
bin/bench --algorithm naive -b benchmark/prefix/path --report results --no-check
```
