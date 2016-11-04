#!/bin/bash

# Automates the creation of n-dimensional query sets
# Usage: generate.sh <#dimensions> <#points> <name>

if (( $# != 3 )); then
	echo "Illegal number of arguments.";
	exit 1;
fi;

DST="benchmarks/$3";

if [ -e "$DST" ]; then
	echo "Benchmark $DST already exists. Aborting.";
	exit 1;
fi;

echo " - Making executables";
make bin/mkdata;
make bin/bench;

echo " - Generating data";
mkdir $DST;
bin/mkdata -d $1 -n $2 > $DST/data.csv;

echo " - Creating query set";
echo "$1	10" > $DST/queries.csv;

for i in {1..10}; do
	echo -n "knn	$i" >> $DST/queries.csv;

	for j in $(seq 1 $1); do
		echo -n "	0.5" >> $DST/queries.csv;
	done;
	echo '' >> $DST/queries.csv;
done;

echo " - Generating valid results";
bin/bench -a naive -b $DST --no-check -r results > $DST/results.csv;

exit 0;
