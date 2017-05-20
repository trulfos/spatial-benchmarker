#pragma once
#include <vector>
#include "MemoryRegion.hpp"

namespace MMap
{

	/**
	 * Represents the memory map as presented by /proc/self/maps.
	 */
	class MemoryMap : std::vector<MemoryRegion>
	{
		public:

			/**
			 * Construct a new memory map by reading /proc/self/maps.
			 */
			MemoryMap();

			using std::vector<MemoryRegion>::begin;
			using std::vector<MemoryRegion>::end;
	};

}
