#include "ReporterArg.hpp"
#include "reporters/RunTimeReporter.hpp"
#include "reporters/SpeedupReporter.hpp"
#include "reporters/ResultsReporter.hpp"



ReporterArg::ReporterArg(
			const std::string& flag,
			const std::string& name,
			const std::string& desc,
			bool req,
			const std::string& value,
			const std::string& typeDesc,
			TCLAP::CmdLineInterface& parser
		)
: TCLAP::ValueArg<std::string>(flag, name, desc, req, value, typeDesc, parser)
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
	} else if (name == "speedup") {
		reporter = std::make_shared<SpeedupReporter>("naive");
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
