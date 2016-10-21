#include "DistributionFactory.hpp"
#include <iostream>
#include <random>
#include <functional>

int main(int argc, char *argv[])
{
	//TODO: Recommended error handling?
	// Construct command line
	TCLAP::CmdLine cmd("Benchmark generator", ' ', "0.1");
	cmd.setExceptionHandling(false);

	// Algorithm selection
	TCLAP::ValueArg<std::string> distribution(
			"d", "distribution", "Distribution to use.", false, "", "distribution"
		);

	TCLAP::ValueArg<unsigned> nPoints(
			"n", "points", "Number of points to generate", false, 100, "filename"
		);

	cmd.add(distribution);
	cmd.add(nPoints);

	// Parse arguments
	try {
		cmd.parse(argc, argv);
	} catch (TCLAP::ArgException &e) {
		// Ups! User input was not correct
		std::cerr << "\033[1;31mError:\033[0m Invalid command line argument"
			<< std::endl << e.what() << std::endl;
		return 1;
	} catch (TCLAP::ExitException &e) {
		// Exited early, e.g. due to --version being used
		return 0;
	}


	// Create data
	std::function<float(std::default_random_engine)> distribution = DistributionFactory::create(distribution.getValue());
	Benchmark benchmark;


	// Output data
	std::string filename = dataFilename.getValue();

	try {
		std::fstream dataFile;
		dataFile.exceptions(std::fstream::badbit | std::fstream::failbit);
		dataFile.open(filename, std::fstream::in);

		dataFile >> benchmark;
	} catch (const std::fstream::failure& e) {
		std::cerr
			<< "\033[1;31mError:\033[0m: I/O error while reading data file!"
			<< std::endl << e.what() << std::endl;
		return 1;
	}

	return 0;
}
