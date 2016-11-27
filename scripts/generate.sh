#!/bin/bash

# Automates the creation of n-dimensional query sets
# Usage: generate.sh <#dimensions> <#coordinates> <query type> <dest dir>

if (( $# != 4 )); then
	echo "Illegal number of arguments.";
	exit 1;
fi;

DST="$4";

if [ -e "$DST" ]; then
	echo "Benchmark $DST already exists. Aborting.";
	exit 1;
fi;

echo " - Making executables";
make bin/mkdata;
make bin/bench;

echo " - Generating data ($(bc <<< "$2/$1") points)";
mkdir -p $DST;
bin/mkdata -s 63 -d $1 -n $(bc <<< "$2/$1") -b > $DST/data.dat;

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
	make bin/mkqueries;
	bin/mkqueries --dimension $1 --selectivity 1e-6 > $DST/queries.csv;
fi;

echo " - Generating valid results";
bin/bench -i naive --no-check -r results $DST/ > $DST/results.csv;

exit 0;
