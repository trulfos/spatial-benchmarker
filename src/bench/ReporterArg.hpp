#pragma once
#include <string>
#include <memory>
#include "reporters/Reporter.hpp"
#include <tclap/UnlabeledMultiArg.h>

/**
 * Argument returning a reporter.
 */
class ReporterArg : protected TCLAP::UnlabeledMultiArg<std::string>
{
	using Base = UnlabeledMultiArg<std::string>;

	public:

		using value_type = std::shared_ptr<Reporter>;
		using container_type = std::vector<value_type>;
		using iterator = container_type::iterator;
		using const_iterator = container_type::const_iterator;

		ReporterArg(
				const std::string& name,
				const std::string& desc,
				bool required,
				const std::string& typeDesc,
				TCLAP::CmdLineInterface& parser
			);

		/**
		 * Customize processing to extract reporters.
		 */
		bool processArg(int *i, std::vector<std::string>& args);


		/**
		 * Get the list of reporters.
		 */
		container_type getValue();

		
		/**
		 * Get list of reporter definitions.
		 */
		Base::container_type getDefinitions();


		/**
		 * Allow iteration through the reporters.
		 */
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;


		// Use some functions from the parent
		using Base::shortID;
		using Base::longID;

	private:

		std::vector<value_type> reporters;

		/**
		 * Create a new reporter from a string.
		 */
		std::shared_ptr<Reporter> createReporter(
				const std::string& value
			);
};
