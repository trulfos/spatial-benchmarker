#include "ReporterArg.hpp"
#include "reporters/RunTimeReporter.hpp"
#include "reporters/SpeedupReporter.hpp"
#include "reporters/ResultsReporter.hpp"

ReporterArg::ReporterArg(
			const std::string& flag,
			const std::string& name,
			const std::string& desc,
			bool req,
			const std::string& typeDesc,
			TCLAP::CmdLineInterface& parser
		)
: TCLAP::ValueArg<std::string>(flag, name, desc, req, "", typeDesc, parser),
	reporter(std::make_shared<RunTimeReporter>())
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
		reporter = std::make_shared<RunTimeReporter>();
	} else if (name.size() >= 7 && name.substr(0, 7) == "speedup") {
		std::string reference = name.size() > 8 && name[7] == '-' ?
			name.substr(8, std::string::npos) : "naive";

		reporter = std::make_shared<SpeedupReporter>(reference);
	} else if (name == "results") {
		reporter = std::make_shared<ResultsReporter>();
	} else {
		throw TCLAP::ArgParseException("No reporter named " + name, toString());
	}

	return true;
}

std::shared_ptr<Reporter> ReporterArg::getValue()
{
	return reporter;
}

void ReporterArg::reset()
{
	TCLAP::ValueArg<std::string>::reset();
	reporter.reset();
}
