#include "../common/Benchmark.hpp"
#include "../common/DataObject.hpp"
#include "../common/DataSet.hpp"
#include "../common/KnnQuery.hpp"
#include "../common/Results.hpp"
#include "../common/QuerySet.hpp"
#include "../common/ResultSet.hpp"
#include "../common/Color.hpp"
#include "Zipped.hpp"
#include "SpatialIndex.hpp"
#include "SpatialIndexFactory.hpp"
#include "Timer.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <tclap/CmdLine.h>


template <typename T>
void readFrom(T& data, std::string filename)
{
	std::fstream file;
	file.exceptions(std::fstream::badbit | std::fstream::failbit);
	file.open(filename, std::fstream::in);
	file >> data;
}

int main(int argc, char *argv[])
{

	// Command line options
	TCLAP::CmdLine cmd("Specialication project test framework", ' ', "0.1");

	TCLAP::MultiArg<std::string> algorithm(
			"a", "algorithm", "Algorithm(s) to run.", false, "algorithm"
		);
	cmd.add(algorithm);

	TCLAP::ValueArg<std::string> dataFilename(
			"b", "benchmark", "Prefix for files containing the benchmark.", true, "", "fileprefix"
		);
	cmd.add(dataFilename);

	TCLAP::SwitchArg generate(
			"g", "generate", "Generate and output result set"
		);
	cmd.add(generate);

	cmd.parse(argc, argv);


	// Read data from file
	DataSet dataSet;
	QuerySet querySet;
	ResultSet resultSet;
	std::string filename = dataFilename.getValue();

	try {
		readFrom(dataSet, filename + ".data.csv");
		readFrom(querySet, filename + ".queries.csv");

		if (generate.getValue()) {
			resultSet.resize(querySet.size());
		} else {
			readFrom(resultSet, filename + ".results.csv");
		}
	} catch (const std::fstream::failure& e) {
		std::cerr
			<< C::red("Error:") << " I/O error while reading data file!"
			<< std::endl << e.what() << std::endl;
		return 1;
	}


	if (querySet.size() != resultSet.size()) {
		std::cerr
			<< C::red("Error:") << " Query and result sets differ in size!"
			<< std::endl;
		return 1;
	}


	// Run code!
	SpatialIndexFactory factory (dataSet);
	Timer timer;

	for (auto alg : algorithm.getValue()) {
		if (!generate.getValue()) {
			std::cout << alg << std::endl;
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
				std::cout << C::red("Error") << "\n"
					<< "Invalid results returned:\n\t" << results << "\n\n"
					<< "Expected:\n\t" << testCase.second << std::endl;
			} else {
				std::cout << time << std::endl;
			}
		}

		if (generate.getValue()) {
			std::cout << resultSet << std::endl;
		}
	}

	return 0;
}
