#include "ReporterFactory.hpp"

#include "RunTimeReporter.hpp"
#include "SpeedupReporter.hpp"
#include "ResultsReporter.hpp"
#include <stdexcept>

std::shared_ptr<Reporter> ReporterFactory::create(std::string reporter)
{
	if (reporter == "runtime") {
		return std::make_shared<RunTimeReporter>();
	}

	if (reporter == "speedup") {
		return std::make_shared<SpeedupReporter>("naive");
	}

	if (reporter == "results") {
		return std::make_shared<ResultsReporter>();
	}

	throw std::logic_error("No reporter named " + reporter);
}

std::ostream& operator<<(
		std::ostream& stream, const std::shared_ptr<Reporter>& reporter
	)
{
	reporter->generate(stream);
	return stream;
}
