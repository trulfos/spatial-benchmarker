#pragma once
#include <limits>
#include <stdexcept>

namespace Rtree
{

/**
 * Basic R-tree node.
 * This is basically a container for entries.
 *
 * @tparam D Dimension
 * @tparam C Node capacity
 * @tparam N Node type
 * @tparam E Entry type
 */
template <unsigned D, unsigned C, class N, template<unsigned, class> class E>
class BaseNode
{
public:
	static constexpr unsigned capacity = C;
	static constexpr unsigned dimension = D;

	using Entry = E<D, N>;

	//TODO: Encapsulate this?
	Entry entries[C];

	/**
	 * Check if this node is full.
	 *
	 * @return True if full
	 */
	bool isFull()
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
	void add(const Entry& entry)
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
	Entry * begin()
	{
		return entries;
	}

	/**
	 * Return the entry past the last entry in this node.
	 */
	Entry * end()
	{
		return entries + nEntries;
	}

private:
	short unsigned nEntries = 0;

};

template<unsigned D, unsigned C, template<unsigned, class> class Entry>
class Node : public BaseNode<D, C, Node<D, C, Entry>, Entry>
{
};

}
