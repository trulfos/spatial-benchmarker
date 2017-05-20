#include "MemoryMap.hpp"
#include <vector>
#include <fstream>

namespace MMap
{

	MemoryMap::MemoryMap()
	{
		std::vector<char> buffer (500);
		std::ifstream maps ("/proc/self/maps");

		// Read all regions
		maps.getline(buffer.data(), buffer.size());
		while (!maps.eof()) {

			if (maps.fail()) {
				//TODO: This can be handled more gracefully
				throw std::runtime_error(
						"Ouch! Could not read all from /proc/self/map! "
						"Maybe line buffer is too small?"
					);
			}

			emplace_back(buffer.data());
			maps.getline(buffer.data(), buffer.size());
		}
	}

}
