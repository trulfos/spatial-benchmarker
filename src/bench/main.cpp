#include "LazyDataSet.hpp"
#include "common/Color.hpp"
#include "SpatialIndex.hpp"
#include "ReporterArg.hpp"
#include "common/Logger.hpp"
#include "DynamicObject.hpp"
#include "reporters/ProgressLogger.hpp"
#include "InvalidStructureError.hpp"
#include <iostream>
#include <string>
#include <tclap/CmdLine.h>

int main(int argc, char *argv[])
{

	Logger logger (std::clog, "Spatial index test framework");

	logger.start("Parsing command line options");
	TCLAP::CmdLine cmd("Specialication project test framework", ' ', "0.5.0");

	TCLAP::UnlabeledValueArg<std::string> algorithm (
			"index",
			"Index to benchmark.",
			true, "", "index name", cmd
		);

	TCLAP::UnlabeledValueArg<std::string> dataFilename (
			"dataset",
			"File with rectangles for data set.",
			true, "", "data set file", cmd
		);

	ReporterArg reporters (
			"reporter",
			"Generate a report in the give style.",
			true, "reporter definition(s)", cmd
		);

	cmd.parse(argc, argv);


	try {
		std::string filename = dataFilename.getValue();

		logger.endStart("Preparing to run " + algorithm.getName());

		// Load benchmark data
		logger.start("Opening data set " + filename);
		LazyDataSet dataSet (filename);

		// Create index
		DynamicObject<SpatialIndex, unsigned, unsigned long long> index (
				"./lib" + algorithm.getValue() + ".so",
				dataSet.getDimension(),
				dataSet.getSize()
			);

		// Index data
		logger.endStart("Inserting data from " + filename);
		ProgressLogger progress (std::clog, dataSet.getSize());

		index->setBounds(dataSet.begin().getBounds()); // <- Hack for Hilbert

		for (const DataObject& object : dataSet) {
			index->insert(object);
			progress.increment();
		}

		logger.endStart("Running index self check");
		try {
			index->checkStructure();
		} catch (const InvalidStructureError& e) {
			std::cerr << C::red("Invalid structure: ") << e.what() << std::endl;
			return 1;
		}

		logger.end();

		// Benchmark
		logger.start("Generating reports");

		for (auto reporter : reporters) {
			logger.endStart("Running reporter...");
			reporter->run(*index, std::clog);
		}

		logger.end();

		// Output reports
		logger.endStart("Generating report");
		for (auto reporter : reporters) {
			std::cout << reporter << std::endl;
		}

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
