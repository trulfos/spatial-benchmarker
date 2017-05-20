#include "MemoryRegion.hpp"
#include <vector>
#include <sstream>

namespace MMap
{
	MemoryRegion::MemoryRegion(const std::string& line)
	{
		// Tokenize the string
		std::istringstream s (line);
		std::vector<std::string> result;
		std::string token;

		while (s >> token) {
			result.push_back(token);
		}

		if (result.size() < 2) {
			throw std::runtime_error(
					"Invalid memory region line:\n" + token
				);
		}

		range = parseRange(result[0]);
		permissions = result[1];

		if (result.size() > 5) {
			name = result[5];
		}
	}


	MemoryRegion::iterator MemoryRegion::begin()
	{
		return iterator(range[0]);
	}


	MemoryRegion::iterator MemoryRegion::end() 
	{
		return iterator(range[1]);
	}


	bool MemoryRegion::isAnonymous()
	{
		return name.empty();
	}


	bool MemoryRegion::isHeap()
	{
		return name == "[heap]";
	}


	bool MemoryRegion::isVvar()
	{
		return name == "[vvar]";
	}


	bool MemoryRegion::hasPath()
	{
		return !isAnonymous() && name[0] != '[';
	}


	std::string MemoryRegion::getPath()
	{
		return (isAnonymous() || name[0] == '[') ? "" : name;
	}


	bool MemoryRegion::isReadable()
	{
		return permissions.size() && permissions[0] == 'r';
	}


	void MemoryRegion::invalidate()
	{
		for (char * address : *this) {
			asm("clflush %0" : /* outputs */ : "m" (*address));
		}
	}


	std::array<char *, 2> MemoryRegion::parseRange(std::string string)
	{
		std::size_t p = string.find('-');

		if (string.size() < p + 2 || p >= string.size()) {
			throw std::runtime_error("Could not parse range " + string);
		}

		return std::array<char *, 2> {{
				reinterpret_cast<char *>(std::stoull(string.substr(0, p), 0, 16)),
				reinterpret_cast<char *>(std::stoull(string.substr(p + 1), 0, 16))
			}};
	}

}
