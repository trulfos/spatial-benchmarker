#pragma once
#include "common/DataObject.hpp"
#include "Mbr.hpp"

namespace Rtree
{

/**
 * An entry in a node of the R-tree.
 *
 * Each entry works like a handle - it contains the MBR of a node and a pointer
 * to the node itself. The MBR is separated from the node to avoid loading the
 * an entire node when only the MBRs should be scanned.
 *
 * @tparam N Node type
 */
template<unsigned D, class N, template<unsigned, class> class Entry>
class BaseEntry
{
	public:
		using Id = DataObject::Id;
		using M = Mbr<D>;
		using Node = N;
		using E = Entry<D, N>;

		using iterator = E *;
		using const_iterator = const E *;

		static constexpr unsigned dimension = D;

		union {
			Id id;
			N * node;
		};

		M mbr;

		/**
		 * Default constructor.
		 */
		BaseEntry() = default;

		/**
		 * Create a new entry from a node.
		 */
		BaseEntry(N * node, M mbr) : node(node), mbr(mbr)
		{
		};

		/**
		 * Create a new entry and initialize MBR and node with the given
		 * entries.
		 *
		 * @param node Pointer to initial node
		 * @param entries Initial entries
		 */
		BaseEntry(N * node, std::initializer_list<E> entries)
			: node(node)
		{
			assign(entries.begin(), entries.end());
		}

		/**
		 * Create a new entry from the given object.
		 */
		BaseEntry(const DataObject& object)
		{
			id = object.getId();
			mbr = object.getBox();
		};

		
		/**
		 * Add an entry as a child of this entry.
		 * The entry will be added to the child node, and the MBR updated.
		 *
		 * @param entry Entry to add
		 */
		void add(const E& entry)
		{
			node->add(entry);
			mbr += entry.mbr;
		};


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
			if (start == end) {
				throw std::logic_error(
						"Entry reset needs at least one child. None given!"
					);
			}

			mbr = start->mbr;
			node->nEntries = 0;

			for (;start != end; ++start) {
				static_cast<E *>(this)->add(*start);
			}
		};


		/**
		 * Remove all entries and add the given initializer list. This also
		 * fixes the MBR.
		 */
		E& operator=(std::initializer_list<E> entries)
		{
			assign(entries.begin(), entries.end());
			return static_cast<E&>(*this);
		};


		/**
		 * Recalculates the MBR of this entry using the MBRs of its children.
		 */
		void recalculateMbr()
		{
			assert(node->nEntries > 0);

			mbr = node->entries[0].mbr;

			for (const E& e : *this) {
				mbr += e.mbr;
			}
		}


		/**
		 * Calculates the overlap of this entry with other entries as defined by
		 * Beckmann et al. for the R*-tree.
		 *
		 * @param first Forward iterator to first element
		 * @param last End forward iterator
		 */
		template<class FI>
		float overlap(FI first, FI last) const
		{
			float result = 0.0f;

			for (;first != last; ++first) {
				result += mbr.intersection(first->mbr).volume();
			}

			return result;
		}


		/**
		 * Return first entry in this entry's node.
		 */
		iterator begin()
		{
			return node->begin();
		};

		/**
		 * Return the entry past the last entry in this entry's node.
		 */
		iterator end()
		{
			return node->end();
		};


		const_iterator cbegin() const
		{
			return node->begin();
		};

		const_iterator cend() const
		{
			return node->end();
		};

};

template<unsigned D, class N>
class Entry : public BaseEntry<D, N, Entry>
{
	public:
		using BaseEntry<D, N, Entry>::BaseEntry;
		using BaseEntry<D, N, Entry>::operator=;
};

}
