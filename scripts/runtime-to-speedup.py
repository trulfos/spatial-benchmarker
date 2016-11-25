#!/usr/bin/python3
import sys


"""
Convert rentime results to speedup results.

This allows running the benchmarker only once to generate runtimes and then
converting the runtimes to speedup for use in the report, thus avoid having to
run the benchmarker twice to get both types of results.

Usage: ./runtime-to-speedup.py <reference index name>

"""


def main():

    if len(sys.argv) != 2:
        sys.exit('Please provide exactly one argument\n')

    # Find reference index
    header = sys.stdin.readline().strip().split()
    index = sys.argv[1]

    try:
        reference = header.index(index)
    except ValueError:
        sys.exit('Index %s not found in header' % index)

    # Output header
    print('\t'.join(header))

    # Convert...
    for line in sys.stdin:
        values = [float(v) for v in line.strip().split()]

        print(values[0], end='\t')
        print('\t'.join(str(values[reference] / v) for v in values[1:]))


main()
