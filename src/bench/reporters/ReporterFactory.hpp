#pragma once
#include <string>
#include <memory>
#include "Reporter.hpp"

/**
 * Generates reporters from string names.
 */
class ReporterFactory
{
	public:
		static std::shared_ptr<Reporter> create(std::string reporter);
};

std::ostream& operator<<(
		std::ostream& stream, const std::shared_ptr<Reporter>& reporter
	);
