#pragma once
#include <string>
#include <memory>
#include "reporters/Reporter.hpp"
#include <tclap/ValueArg.h>

/**
 * Argument returning a reporter.
 */
class ReporterArg : private TCLAP::ValueArg<std::string>
{

	public:
		using TCLAP::ValueArg<std::string>::shortID;
		using TCLAP::ValueArg<std::string>::longID;

		ReporterArg(
				const std::string& flag,
				const std::string& name,
				const std::string& desc,
				bool req,
				const std::string& typeDesc,
				TCLAP::CmdLineInterface& parser
			);

		bool processArg(int *i, std::vector<std::string>& args);

		std::shared_ptr<Reporter> getValue();

		void reset();

	private:
		std::shared_ptr<Reporter> reporter;
};
