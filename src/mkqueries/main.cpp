#include "common/QuerySet.hpp"
#include "common/RangeQuery.hpp"
#include "common/KnnQuery.hpp"
#include <iostream>
#include <random>
#include <tclap/CmdLine.h>
#include <cmath>

int main(int argc, char *argv[])
{
	// Construct command line
	TCLAP::CmdLine cmd("Benchmark query generator", ' ', "0.1");

	TCLAP::ValueArg<unsigned> nQueries(
			"n", "queries",
			"Number of queries to generate",
			false, 20, "number", cmd
		);

	TCLAP::ValueArg<unsigned> dimension(
			"d", "dimension",
			"Dimension of data set",
			false, 3, "number", cmd
		);

	TCLAP::ValueArg<unsigned> seed(
			"s", "seed" ,
			"Seed for the random engine",
			false, 13, "number", cmd
		);

	TCLAP::ValueArg<float> selectivity(
			"q", "selectivity",
			"Selectivity of queries to generate",
			false, 1e-6, "float", cmd
		);

	TCLAP::ValueArg<unsigned> precision(
			"p", "precision",
			"Number of decimals to output",
			false, 10, "number", cmd
		);

	cmd.parse(argc, argv);


	// Generate data
	QuerySet querySet;
	unsigned d = dimension.getValue();
	unsigned n = nQueries.getValue();
	float size = std::pow(selectivity.getValue(), 1.0f / d);
	std::default_random_engine engine (seed.getValue());

	std::uniform_real_distribution<float> uniform (
			0.0f + size/2,
			1.0f - size/2
		);

	while (n--) {
		Point center (d, uniform, engine);
		querySet.add(
				RangeQuery(
					Box(center, size)
				)
			);
	}

	// Print benchmark
	std::cout
		<< std::fixed
		<< std::setprecision(precision.getValue())
		<< querySet
		<< std::endl;

	return 0;
}
