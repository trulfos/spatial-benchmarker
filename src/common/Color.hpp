#pragma once
#include <string>

/**
 * Simple namespace with color constants.
 */
namespace C {

	/**
	 * Colors the text red
	 */
	std::string red(std::string string)
	{
		return "\033[1;31m" + string + "\033[0m\n";
	}

}
