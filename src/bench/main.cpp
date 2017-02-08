#include "LazyDataSet.hpp"
#include "LazyQuerySet.hpp"
#include "common/Results.hpp"
#include "common/ResultSet.hpp"
#include "common/Color.hpp"
#include "Zipped.hpp"
#include "SpatialIndex.hpp"
#include "ReporterArg.hpp"
#include "common/Logger.hpp"
#include "common/ProgressLogger.hpp"
#include "Benchmark.hpp"
#include "DynamicObject.hpp"
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

	Logger logger (std::clog, "Spatial index test framework");


	logger.start("Parsing command line options");

	TCLAP::CmdLine cmd("Specialication project test framework", ' ', "0.4.0");

	ReporterArg reportType(
			"r", "report",
			"Generate a report in the give style.",
			false, "report style", cmd
		);

	TCLAP::SwitchArg noCheck(
			"n", "no-check",
			"Don't check the results. This avoids the need for a results file.",
			cmd
		);

	TCLAP::UnlabeledValueArg<std::string> algorithm(
			"index",
			"Index to benchmark.",
			true, "", "index name", cmd
		);

	TCLAP::UnlabeledValueArg<std::string> dataFilename(
			"benchmark",
			"Folder containing benchmark data.",
			true, "", "benchmark folder", cmd
		);

	cmd.parse(argc, argv);


	try {
		std::string filename = dataFilename.getValue();

		logger.endStart("Preparing to run " + algorithm.getName());

		logger.start("Opening benchmark " + filename);

		Benchmark benchmark (filename);

		// Load benchmark data
		LazyDataSet dataSet = benchmark.getData();
		LazyQuerySet querySet = benchmark.getQueries();
		ResultSet resultSet = benchmark.getResults(noCheck.getValue());

		// Create reporter and index
		auto reporter = reportType.getValue();
		//auto index = algorithm.getValue(dataSet.getDimension());
		DynamicObject<SpatialIndex> index ("./" + algorithm.getValue() + ".so");

		// Index data
		logger.endStart("Indexing " + filename + "data");
		index->load(dataSet);

		logger.end();

		// Benchmark
		logger.endStart("Benchmarking");
		ProgressLogger progress(std::clog, querySet.getSize());

		for (auto testCase : zip(querySet, resultSet)) {

			// Do the search
			Results results = reporter->run(
					algorithm.getName(),
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

			progress.increment();
		}

		// Output report
		logger.endStart("Generating report");
		std::cout << reporter;

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
