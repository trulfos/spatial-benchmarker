#pragma once
#include "Entry.hpp"
#include "Mbr.hpp"
#include <limits>

namespace Rtree
{

template <unsigned D, unsigned C>
class Node
{
public:
	using E = Entry<D, C>;
	using M = Mbr<D>;

	E entries[C];
	short unsigned nEntries = 0;

	/**
	 * Check if this node is full.
	 *
	 * @return True if full
	 */
	bool isFull()
	{
		return nEntries == C;
	};


	/**
	 * Add an entry to this node.
	 *
	 * @return True if successful (not full)
	 */
	void add(const E& entry)
	{
		if (nEntries >= C) {
			throw std::out_of_range(
					"Trying to add entry to full node."
				);
		}

		entries[nEntries++] = entry;
	};


	/**
	 * Reset this node so it only contains the given entry.
	 *
	 * @param entry Entry it should contain
	 */
	void reset(const E& entry)
	{
		entries[0] = entry;
		nEntries = 1;
	}


	/**
	 * Return first entry in this node.
	 */
	E * begin()
	{
		return entries;
	};

	/**
	 * Return the entry past the last entry in this node.
	 */
	E * end()
	{
		return entries + nEntries;
	};
};

}
