#include "CacheLineIterator.hpp"

namespace MMap
{
	CacheLineIterator::CacheLineIterator(char * start)
		: current(start)
	{
	}

	bool CacheLineIterator::operator==(const CacheLineIterator& other) const
	{
		return current == other.current;
	}

	bool CacheLineIterator::operator!=(const CacheLineIterator& other) const
	{
		return !(*this == other);
	}

	CacheLineIterator::reference CacheLineIterator::operator*()
	{
		return current;
	}

	CacheLineIterator::pointer CacheLineIterator::operator->()
	{
		return &operator*();
	}

	CacheLineIterator& CacheLineIterator::operator++()
	{
		current += CACHE_LINE_SIZE;
		return *this;
	}

	CacheLineIterator CacheLineIterator::operator++(int)
	{
		CacheLineIterator it = *this;
		operator++();
		return it;
	}
}
