#include "DataSet.hpp"
#include <iostream>
#include <random>
#include <functional>
#include <tclap/CmdLine.h>

int main(int argc, char *argv[])
{
	// Construct command line
	TCLAP::CmdLine cmd("Benchmark data generator", ' ', "0.1");

	/*
	 * TODO:
	TCLAP::ValueArg<std::string> distribution(
			"p", "distribution", "Probability distribution to use.", false, "", "distribution"
		);
	cmd.add(distribution);
	*/

	TCLAP::ValueArg<unsigned> nPoints(
			"n", "points", "Number of points to generate", false, 100, "number"
		);
	cmd.add(nPoints);

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

	DataSet dataSet;
	unsigned d = dimension.getValue();


	for (unsigned j = 0; j < nPoints.getValue(); j++) {
		dataSet.push_back(
				DataObject(j + 1, Point(d, uniform, engine))
			);
	}

	// Print benchmark
	std::cout << dataSet << std::endl;

	return 0;
}
