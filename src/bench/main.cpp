#include "../common/Benchmark.hpp"
#include "../common/DataObject.hpp"
#include "../common/DataSet.hpp"
#include "../common/KnnQuery.hpp"
#include "../common/Results.hpp"
#include "../common/QuerySet.hpp"
#include "../common/ResultSet.hpp"
#include "Zipped.hpp"
#include "SpatialIndex.hpp"
#include "SpatialIndexFactory.hpp"
#include "Timer.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <tclap/CmdLine.h>


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
	cmd.add(algorithm);

	TCLAP::ValueArg<std::string> dataFilename(
			"b", "benchmark", "File containing benchmark.", true, "", "filename"
		);
	cmd.add(dataFilename);

	TCLAP::SwitchArg generate(
			"g", "generate", "Generate and output result set"
		);
	cmd.add(generate);


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
	DataSet dataSet;
	QuerySet querySet;
	ResultSet resultSet;
	std::string filename = dataFilename.getValue();

	try {
		// Read data
		std::fstream dataFile;
		dataFile.exceptions(std::fstream::badbit | std::fstream::failbit);
		dataFile.open(filename + ".data.csv", std::fstream::in);
		dataFile >> dataSet;

		// Read queries
		std::fstream queryFile;
		queryFile.exceptions(std::fstream::badbit | std::fstream::failbit);
		queryFile.open(filename + ".queries.csv", std::fstream::in);
		queryFile >> querySet;

		// Read or generate result set
		if (!generate.getValue()) {
			std::fstream resultFile;
			resultFile.exceptions(std::fstream::badbit | std::fstream::failbit);
			resultFile.open(filename + ".results.csv", std::fstream::in);
			resultFile >> resultSet;
		} else {
			resultSet.resize(querySet.size());
		}


	} catch (const std::fstream::failure& e) {
		std::cerr
			<< "\033[1;31mError:\033[0m: I/O error while reading data file!"
			<< std::endl << e.what() << std::endl;
		return 1;
	}


	if (querySet.size() != resultSet.size()) {
		std::cerr
			<< "\033[1;31mError:\033[0m: Query and result sets differ in size!"
			<< std::endl;
		return 1;
	}


	// Run code!
	SpatialIndexFactory factory (dataSet);
	Timer timer;

	for (auto alg : algorithm.getValue()) {
		if (!generate.getValue()) {
			std::cout << "Testing " << alg << std::endl
				<< " - Constructing index" << std::endl;
		}

		auto index = factory.create(alg);

		for (auto testCase : zip(querySet, resultSet)) {
			Results results;
			unsigned long time = timer.timeTask([&]() -> void {
				results = index->search(testCase.first);
			});

			std::sort(results.begin(), results.end());

			if (generate.getValue()) {
				testCase.second = results;
				continue;
			}

			if (results != testCase.second) {
				std::cout << "\033[1;31mError\033[0m" << std::endl
					<< "Invalid results returned: " << std::endl
					<< "  " << results << std::endl << std::endl
					<< "Expected:" << std::endl
					<< "  " << testCase.second << std::endl;
			} else {
				std::cout << "\033[1;32mSuccess!\033[0m (" << time << " µs)"
					<< std::endl;
			}
		}

		if (generate.getValue()) {
			std::cout << resultSet << std::endl;
		}
	}

	return 0;
}
