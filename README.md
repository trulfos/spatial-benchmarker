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
