#include "DataSet.hpp"
#include <iostream>
#include <random>
#include <functional>
#include <string>
#include <fstream>
#include <tclap/CmdLine.h>

int main(int argc, char *argv[])
{
	// Construct command line
	TCLAP::CmdLine cmd("Benchmark data generator", ' ', "0.2");

	TCLAP::ValueArg<std::string> input(
			"i", "input",
			"Read data set from file",
			false, "", "file", cmd
		);

	TCLAP::ValueArg<unsigned> nPoints(
			"n", "points",
			"Number of points to generate",
			false, 100, "number", cmd
		);

	TCLAP::ValueArg<unsigned> dimension(
			"d", "dimension",
			"Dimension of data set",
			false, 3, "number", cmd
		);

	TCLAP::ValueArg<unsigned> seed(
			"s", "seed" ,
			"Seed for the random engine",
			false, 11, "number", cmd
		);

	TCLAP::ValueArg<unsigned> precision(
			"p", "precision",
			"Number of decimals to output",
			false, 10, "number", cmd
		);

	TCLAP::SwitchArg binary(
			"b", "binary", "Output binary data", cmd
		);


	cmd.parse(argc, argv);


	// Create data
	std::default_random_engine engine (seed.getValue());
	std::uniform_real_distribution<float> uniform;

	DataSet dataSet;
	std::string infile = input.getValue();

	if (infile.empty()) {
		unsigned d = dimension.getValue();
		for (unsigned j = 0; j < nPoints.getValue(); j++) {
			dataSet.push_back(
					DataObject(j + 1, Point(d, uniform, engine))
				);
		}
	} else {
		std::fstream stream(infile, std::fstream::in);
		stream >> dataSet;
	}

	// Print benchmark
	if (binary.getValue()) {
		dataSet.write(std::cout);
	} else {
		std::cout
			<< std::fixed
			<< std::setprecision(precision.getValue())
			<< dataSet
			<< std::endl;
	}

	return 0;
}
