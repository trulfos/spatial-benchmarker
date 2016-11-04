#include "common/QuerySet.hpp"
#include "common/RangeQuery.hpp"
#include "common/KnnQuery.hpp"
#include <iostream>
#include <random>
#include <tclap/CmdLine.h>

int main(int argc, char *argv[])
{
	// Construct command line
	TCLAP::CmdLine cmd("Benchmark query generator", ' ', "0.1");

	TCLAP::ValueArg<unsigned> nQueries(
			"n", "queries", "Number of queries to generate", false, 100, "number"
		);
	cmd.add(nQueries);

	TCLAP::ValueArg<unsigned> dimension(
			"d", "dimension", "Dimension of data set", false, 3, "number"
		);
	cmd.add(dimension);

	TCLAP::ValueArg<unsigned> seed(
			"s", "seed" , "Seed for the random engine", false, 11, "number"
		);
	cmd.add(seed);

	cmd.parse(argc, argv);


	// Create data
	std::default_random_engine engine (seed.getValue());
	std::uniform_real_distribution<float> uniform;
	std::uniform_int_distribution<unsigned> uniInt (0, nQueries.getValue());

	QuerySet querySet;
	unsigned d = dimension.getValue();

	for (unsigned j = 0; j < nQueries.getValue(); j++) {
		if (uniform(engine) > 0.5) {
			querySet.add(
					KnnQuery(uniInt(engine), Point(d, uniform, engine))
				);
		} else {
			querySet.add(
					RangeQuery(
						Point(d, uniform, engine),
						Point(d, uniform, engine)
					)
				);
		}
	}

	// Print benchmark
	std::cout << querySet << std::endl;

	return 0;
}
