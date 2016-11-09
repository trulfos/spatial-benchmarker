#!/bin/bash

# Automates the creation of n-dimensional query sets
# Usage: generate.sh <#dimensions> <#points> <name>

if (( $# != 4 )); then
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

if [ "$3" == "knn" ]; then
	echo "$1	10" > $DST/queries.csv;

	for i in {1..10}; do
		echo -n "knn	$i" >> $DST/queries.csv;

		for j in $(seq 1 $1); do
			echo -n "	0.5" >> $DST/queries.csv;
		done;
		echo '' >> $DST/queries.csv;
	done;
else
	echo "$1	5" > $DST/queries.csv;
	
	for i in 0.45 0.46 0.47 0.48 0.49; do
		echo -n "range" >> $DST/queries.csv;

		for j in $(seq 1 $1); do
			echo -n "	$i" >> $DST/queries.csv;
		done;

		for j in $(seq 1 $1); do
			echo -n "	"$(echo "scale=1;1-$i" | bc) >> $DST/queries.csv;
		done;

		echo '' >> $DST/queries.csv;
	done;
fi;

echo " - Generating valid results";
bin/bench -i naive --no-check -r results $DST > $DST/results.csv;

exit 0;
