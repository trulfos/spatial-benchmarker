#pragma once
#include <string>

/**
 * Simple namespace with color constants.
 */
namespace C {

	/**
	 * Colors the text red
	 */
	inline
	std::string red(std::string string)
	{
		return "\033[1;31m" + string + "\033[0m";
	}

	/**
	 * Colors the text green
	 */
	inline
	std::string green(std::string string)
	{
		return "\033[1;32m" + string + "\033[0m";
	}

	/**
	 * Makes text bold (not strictly a color)
	 */
	inline
	std::string bold(std::string string)
	{
		return "\033[1;1m" + string + "\033[0m";
	}
}
