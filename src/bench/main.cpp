#include "../common/DataObject.hpp"
#include "LazyDataSet.hpp"
#include "../common/KnnQuery.hpp"
#include "../common/Results.hpp"
#include "../common/QuerySet.hpp"
#include "../common/ResultSet.hpp"
#include "../common/Color.hpp"
#include "Zipped.hpp"
#include "SpatialIndex.hpp"
#include "SpatialIndexFactory.hpp"
#include "ReporterArg.hpp"
#include "../common/Logger.hpp"
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

	Logger logger (std::clog, "Specialication project test framework");

	logger.start("Parsing command line arguments");

	// Command line options
	TCLAP::CmdLine cmd("Specialication project test framework", ' ', "0.2.0");

	TCLAP::MultiArg<std::string> algorithm(
			"a", "algorithm",
			"Algorithm(s) to run.",
			false, "algorithm", cmd
		);

	TCLAP::ValueArg<std::string> dataFilename(
			"b", "benchmark",
			"Folder containing benchmark data.",
			true, "", "fileprefix", cmd
		);

	ReporterArg reportType(
			"r", "report",
			"Generate a report in the give style.",
			false, "style", cmd
		);

	TCLAP::SwitchArg noCheck(
			"n", "no-check",
			"Don't check the results. This avoids the need for a results file.",
			cmd
		);

	cmd.parse(argc, argv);

	try {
		QuerySet querySet;
		ResultSet resultSet;
		std::string filename = dataFilename.getValue();

		logger.endStart("Reading queries and results from " + filename);

		readFrom(querySet, filename + "/queries.csv");

		if (!noCheck.getValue()) {
			readFrom(resultSet, filename + "/results.csv");

			if (querySet.size() != resultSet.size()) {
				throw std::logic_error("Query and result sets differ in size!");
			}
		}

		// Run code!
		auto reporter = reportType.getValue();

		for (auto alg : algorithm.getValue()) {
			logger.endStart("Benchmarking " + alg);
			logger.start("Indexing " + filename + "/data.csv");

			LazyDataSet dataSet (filename + "/data.csv");
			auto index = SpatialIndexFactory::create(alg, dataSet);

			for (auto testCase : zip(querySet, resultSet)) {
				logger.endStart("Generating data for query");

				// Do the search
				Results results = reporter->run(
						alg,
						testCase.first,
						*index.get()
					);

				// Check results
				if (!noCheck.getValue()) {
					std::sort(results.begin(), results.end());

					if (results != testCase.second) {
						std::cerr << C::red("Error") << "\n"
							<< "Invalid results returned:\n\t" << results
							<< "\n\nExpected:\n\t" << testCase.second
							<< std::endl;
					}
				}
			}

			logger.end();
		}


		logger.endStart("Generating report");
		std::cout << reporter << std::endl;

		return 0;

	} catch (const std::fstream::failure& e) {
		std::cerr << C::red("I/O error:") << '\n' << e.what() << std::endl;
	} catch (const std::logic_error& e) {
		std::cerr << C::red("Logic error:") << '\n' << e.what() << std::endl;
	} catch (const std::bad_alloc& e) {
		std::cerr << C::red("Bad allocation:") << '\n' << e.what() << std::endl;
	}

	return 1;
}
