#pragma once
#include <limits>
#include <stdexcept>
#include "Entry.hpp"

namespace Rtree
{

/**
 * Basic R-tree node.
 *
 * This is basically a container for entries. Extra data may be stored in a node
 * using the additional type parameter.
 *
 * @tparam D Dimension
 * @tparam C Node capacity
 * @tparam P Entry plugin
 */
template <
		unsigned D,
		unsigned C,
		template<class> class P = EntryPlugin
	>
class Node
{
public:
	static constexpr unsigned capacity = C;
	static constexpr unsigned dimension = D;

	using E = Entry<D, Node, P>;

	using iterator = E *;
	using const_iterator = const E *;

	// Additional data field
	typename E::Plugin::NodeData data;

	/**
	 * Check if this node is full.
	 *
	 * @return True if full
	 */
	bool isFull() const
	{
		return nEntries == C;
	}


	/**
	 * Find the number of entries currently present in this node.
	 */
	unsigned size() const
	{
		return nEntries;
	}


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
	}


	/**
	 * Sets the number of entries in this node to 0.
	 */
	void reset()
	{
		nEntries = 0;
	}


	/**
	 * Return first entry in this node.
	 */
	E * begin()
	{
		return entries;
	}

	/**
	 * Return the entry past the last entry in this node.
	 */
	E * end()
	{
		return entries + nEntries;
	}

private:
	short unsigned nEntries = 0;
	E entries[C];


};

}
