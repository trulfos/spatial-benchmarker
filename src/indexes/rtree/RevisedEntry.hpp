#pragma once
#include "Entry.hpp"

namespace Rtree
{

/**
 * Entry for the revised R*-tree.
 *
 * Automatically captures the MBR when assigned to.
 */
template<unsigned D, class N>
class RevisedEntry : public BaseEntry<D, N, RevisedEntry>
{

	using Base = BaseEntry<D, N, RevisedEntry>;
	using E = typename Base::E;

	public:
		using BaseEntry<D, N, RevisedEntry>::BaseEntry;
		using BaseEntry<D, N, RevisedEntry>::operator=;

		RevisedEntry() = default;

		/**
		 * Create a new entry and initialize MBR and node with the given
		 * entries.
		 *
		 * @param node Pointer to initial node
		 * @param entries Initial entries
		 */
		RevisedEntry(N * node, std::initializer_list<E> entries)
			: Base(node, entries)
		{
			this->getNode()->captureMbr();
		}


		/**
		 * Remove all entries and add from the given iterators. This also fixes
		 * the MBR.
		 *
		 * @param start First element
		 * @param end Element beyond the last
		 */
		template<class ForwardIterator>
		void assign(ForwardIterator start, ForwardIterator end)
		{
			Base::assign(start, end);
			this->getNode()->captureMbr();
		};


		/**
		 * Remove all entries and add the given initializer list. This also
		 * fixes the MBR.
		 */
		E& operator=(std::initializer_list<E> entries)
		{
			Base::operator=(entries);
			this->getNode()->captureMbr();
		};

};

}
