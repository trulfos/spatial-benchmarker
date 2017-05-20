#pragma once
#include <string>
#include <array>
#include "CacheLineIterator.hpp"

namespace MMap
{
	class MemoryRegion
	{


		public:
			using iterator = CacheLineIterator;

			/**
			 * TODO:
			 */
			explicit MemoryRegion(const std::string& line);

			iterator begin();

			iterator end();


			bool isAnonymous();


			bool isHeap();

			bool isVvar();

			bool hasPath();

			std::string getPath();

			bool isReadable();

			void invalidate();

		private:
			std::array<char *, 2> range;
			std::string name, permissions;

			/**
			 * TODO:
			 */
			std::array<char *, 2> parseRange(std::string string);
	};
}
