#include "ReporterArg.hpp"
#include "reporters/TotalRunTimeReporter.hpp"
#include "reporters/QueryRunTimeReporter.hpp"
#include "reporters/ResultsReporter.hpp"
#include "reporters/StatsReporter.hpp"
#include "reporters/AvgStatsReporter.hpp"
#include "reporters/CorrectnessReporter.hpp"
#include "reporters/StructReporter.hpp"

ReporterArg::ReporterArg(
			const std::string& name,
			const std::string& desc,
			bool required,
			const std::string& typeDesc,
			TCLAP::CmdLineInterface& parser
		)
	: Base(name, desc, required, typeDesc, parser)
{
}


bool ReporterArg::processArg(int *i, std::vector<std::string>& args)
{
	// Leave "heavy lifting" to base
	if (!Base::processArg(i, args)) {
		return false;
	}

	// Create reporter
	reporters.push_back(
			createReporter(args[*i])
		);

	return true;
}



ReporterArg::container_type ReporterArg::getValue()
{
	return reporters;
}

ReporterArg::Base::container_type ReporterArg::getDefinitions()
{
	return Base::getValue();
}

ReporterArg::iterator ReporterArg::begin()
{
	return reporters.begin();
}


ReporterArg::iterator ReporterArg::end()
{
	return reporters.end();
}


ReporterArg::const_iterator ReporterArg::begin() const
{
	return reporters.begin();
}


ReporterArg::const_iterator ReporterArg::end() const
{
	return reporters.end();
}


std::shared_ptr<Reporter> ReporterArg::createReporter(
		const std::string& value
	)
{
	// Separate reporter name from extra arguments
	size_t splitPoint = value.find(':');

	std::string name = value.substr(0, splitPoint);
	std::vector<std::string> arguments;

	size_t base = splitPoint;

	while (base != std::string::npos) {
		splitPoint = value.find(':', base + 1);
		arguments.push_back(value.substr(base + 1, splitPoint - base - 1));
		base = splitPoint;
	}

	// Instantiate reporter
	if (name == "runtime") {
		return std::make_shared<TotalRunTimeReporter>(arguments[0]);
	}
	if (name == "qruntime") {
		return std::make_shared<QueryRunTimeReporter>(arguments[0]);
	}
	if (name == "results") {
		return std::make_shared<ResultsReporter>(arguments[0]);
	}
	if (name == "stats") {
		return std::make_shared<AvgStatsReporter>(arguments[0]);
	}
	if (name == "qstats") {
		return std::make_shared<StatsReporter>(arguments[0]);
	}
	if (name == "correctness") {
		return std::make_shared<CorrectnessReporter>(arguments[0], arguments[1]);
	}
	if (name == "struct") {
		return std::make_shared<StructReporter>();
	}

	throw TCLAP::ArgParseException("No reporter named " + name, "reporterarg");
}
