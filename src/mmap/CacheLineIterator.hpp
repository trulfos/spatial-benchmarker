#pragma once
#include <iterator>

namespace MMap
{
	class CacheLineIterator
	{
		public:
			using value_type = char *;
			using reference = char *const &;
			using pointer = char *const *;
			using category = std::input_iterator_tag;

			static constexpr unsigned CACHE_LINE_SIZE = 64;


			CacheLineIterator() = default;

			explicit CacheLineIterator(char * start);
			bool operator==(const CacheLineIterator& other) const;
			bool operator!=(const CacheLineIterator& other) const;
			reference operator*();
			pointer operator->();
			CacheLineIterator& operator++();
			CacheLineIterator operator++(int);

		private:
			char * current = 0;
	};
}
