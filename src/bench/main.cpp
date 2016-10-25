#include "../common/Benchmark.hpp"
#include "../common/DataObject.hpp"
#include "../common/DataSet.hpp"
#include "../common/KnnQuery.hpp"
#include "../common/ResultSet.hpp"
#include "SpatialIndex.hpp"
#include "SpatialIndexFactory.hpp"
#include "Timer.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <tclap/CmdLine.h>


/**
 * For convenience
 */
template<typename T>
std::ostream& operator<<(std::ostream& stream, std::vector<T> vector)
{
	stream << "std::vector(" << vector.size() << "){ ";
	for (auto element : vector) {
		stream << element << ' ';
	}

	return stream << "}";
}

int main(int argc, char *argv[])
{

	//TODO: Recommended error handling?
	// Construct command line
	TCLAP::CmdLine cmd("Specialication project test framework", ' ', "0.1");
	cmd.setExceptionHandling(false);

	// Algorithm selection
	TCLAP::MultiArg<std::string> algorithm(
			"a", "algorithm", "Algorithm(s) to run.", false, "algorithm"
		);

	TCLAP::ValueArg<std::string> dataFilename(
			"b", "benchmark", "File containing benchmark.", true, "", "filename"
		);

	cmd.add(algorithm);
	cmd.add(dataFilename);

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


	// Read data from file
	Benchmark benchmark;
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


	// Run code!
	SpatialIndexFactory factory (benchmark.getDataSet());
	Timer timer;

	for (auto alg : algorithm.getValue()) {
		std::cout << "Testing " << alg << std::endl
			<< " - Constructing index" << std::endl;

		auto index = factory.create(alg);

		for (auto testCase : benchmark.getTestCases()) {

			std::cout << " - Running test case... ";
			ResultSet results;
			unsigned long time = timer.timeTask([&]() -> void {
				results = index->search(*testCase.first);
			});

			std::sort(results.begin(), results.end());

			if (results != *testCase.second) {
				std::cout << "\033[1;31mError\033[0m" << std::endl
					<< "Invalid results returned: " << std::endl
					<< "  " << results << std::endl << std::endl
					<< "Expected:" << std::endl
					<< "  " << *testCase.second << std::endl;
			} else {
				std::cout << "\033[1;32mSuccess!\033[0m (" << time << " µs)"
					<< std::endl;
			}
		}
	}

	return 0;
}