#include "../common/DataObject.hpp"
#include "LazyDataSet.hpp"
#include "reporters/ReporterFactory.hpp"
#include "../common/KnnQuery.hpp"
#include "../common/Results.hpp"
#include "../common/QuerySet.hpp"
#include "../common/ResultSet.hpp"
#include "../common/Color.hpp"
#include "Zipped.hpp"
#include "SpatialIndex.hpp"
#include "SpatialIndexFactory.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <tclap/CmdLine.h>

const unsigned CACHE_SIZE = 4096; // kilobytes
const unsigned CACHE_LINE_SIZE = 64; // bytes


template <typename T>
void readFrom(T& data, std::string filename)
{
	std::fstream file;
	file.exceptions(std::fstream::badbit | std::fstream::failbit);
	file.open(filename, std::fstream::in);
	file >> data;
}


/**
 * "Clears" the cache by writing data to a large array.
 * To save time, only one value in each cache line is written.
 */
void clearCache()
{
	unsigned size = CACHE_SIZE * 1024;
	char * buffer = new char[CACHE_SIZE * 1024];

	// write bullshit
	for (unsigned i = 0; i < size; i += CACHE_LINE_SIZE) {
		buffer[i] = (char) i;
	}

	delete[] buffer;
}

int main(int argc, char *argv[])
{

	// Command line options
	TCLAP::CmdLine cmd("Specialication project test framework", ' ', "0.2.0");

	TCLAP::MultiArg<std::string> algorithm(
			"a", "algorithm",
			"Algorithm(s) to run.",
			false, "algorithm", cmd
		);

	TCLAP::ValueArg<std::string> dataFilename(
			"b", "benchmark",
			"Prefix for files containing the benchmark.",
			true, "", "fileprefix", cmd
		);

	TCLAP::ValueArg<std::string> reportType(
			"r", "report",
			"Generate a report in the give style.",
			false, "runtime", "style"
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

		readFrom(querySet, filename + ".queries.csv");

		if (noCheck.getValue()) {
			resultSet.resize(querySet.size());
		} else {
			readFrom(resultSet, filename + ".results.csv");

			if (querySet.size() != resultSet.size()) {
				throw std::logic_error("Query and result sets differ in size!");
			}
		}


		// Run code!
		auto reporter = ReporterFactory::create(reportType.getValue());

		for (auto alg : algorithm.getValue()) {

			LazyDataSet dataSet (filename + ".data.csv");
			auto index = SpatialIndexFactory::create(alg, dataSet);

			for (auto testCase : zip(querySet, resultSet)) {

				clearCache();

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
		}

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
