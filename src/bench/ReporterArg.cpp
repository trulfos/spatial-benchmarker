#include "ReporterArg.hpp"
#include "reporters/TotalRunTimeReporter.hpp"
#include "reporters/QueryRunTimeReporter.hpp"
#include "reporters/ResultsReporter.hpp"
#include "reporters/StatsReporter.hpp"
#include "reporters/AvgStatsReporter.hpp"
#include "reporters/CorrectnessReporter.hpp"

ReporterArg::ReporterArg(
			const std::string& flag,
			const std::string& name,
			const std::string& desc,
			bool req,
			const std::string& typeDesc,
			TCLAP::CmdLineInterface& parser
		)
	: TCLAP::ValueArg<std::string>(flag, name, desc, req, "", typeDesc, parser),
		reporter(std::make_shared<TotalRunTimeReporter>())
{
}


bool ReporterArg::processArg(int *i, std::vector<std::string>& args)
{
	bool success = TCLAP::ValueArg<std::string>::processArg(i, args);

	if (!success) {
		return false;
	}

	std::string name = TCLAP::ValueArg<std::string>::getValue();

	if (name == "runtime") {
		reporter = std::make_shared<TotalRunTimeReporter>();
	} else if (name == "qruntime") {
		reporter = std::make_shared<QueryRunTimeReporter>();
	} else if (name == "results") {
		reporter = std::make_shared<ResultsReporter>();
	} else if (name == "stats") {
		reporter = std::make_shared<AvgStatsReporter>();
	} else if (name == "qstats") {
		reporter = std::make_shared<StatsReporter>();
	} else if (name == "correctness") {
		reporter = std::make_shared<CorrectnessReporter>();
	} else {
		throw TCLAP::ArgParseException("No reporter named " + name, toString());
	}

	return true;
}

std::shared_ptr<Reporter> ReporterArg::getValue()
{
	return reporter;
}


std::string ReporterArg::getName()
{
	return TCLAP::ValueArg<std::string>::getValue();
}

void ReporterArg::reset()
{
	TCLAP::ValueArg<std::string>::reset();
	reporter.reset();
}
