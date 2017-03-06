#pragma once
#include <stdexcept>

/**
 * Error thrown when the structure of the index is not correct.
 */
class InvalidStructureError : public std::logic_error
{
	public:
		using std::logic_error::logic_error;
};
