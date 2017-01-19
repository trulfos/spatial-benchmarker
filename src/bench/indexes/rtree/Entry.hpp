#pragma once
#include "common/DataObject.hpp"
#include "Mbr.hpp"

namespace Rtree
{

template<unsigned D, unsigned C>
class Node;

template<unsigned D, unsigned C>
class Entry
{
	public:
		using Id = DataObject::Id;
		using M = Mbr<D>;
		using N = Node<D, C>;

		static const unsigned capacity = C;
		static const unsigned dimension = D;

		union {
			Id id;
			N * node;
		};

		M mbr;

		/**
		 * Default constructor.
		 */
		Entry() = default;

		/**
		 * Create a new entry from a node.
		 */
		Entry(N * node, M mbr) : node(node), mbr(mbr)
		{
		};

		/**
		 * Create a new entry and initialize MBR and node with the given
		 * entries.
		 *
		 * @param node Pointer to initial node
		 * @param entries Initial entries
		 */
		Entry(N * node, std::initializer_list<Entry> entries)
			: node(node)
		{
			if (entries.begin() == entries.end()) {
				throw new std::logic_error(
						"Entry constructor needs entries or MBRs. None given!"
					);
			}

			mbr = entries.begin()->mbr;
			for (auto i = entries.begin(); i != entries.end(); i++) {
				node->add(*i);
			}
		}

		/**
		 * Create a new entry from the given object.
		 */
		Entry(const DataObject& object)
		{
			id = object.getId();
			mbr = object.getPoint();
		};

		
		/**
		 * Add an entry as a child of this entry.
		 * The entry will be added to the child node, and the MBR updated.
		 *
		 * @param entry Entry to add
		 */
		void add(const Entry& entry)
		{
			node->add(entry);
			mbr += entry.mbr;
		};


		/**
		 * Remove all entries and add from the given iterators. This also fixes
		 * the MBR.
		 */
		template<class ForwardIterator>
		void assign(ForwardIterator start, ForwardIterator end)
		{
			if (start == end) {
				throw new std::logic_error(
						"Entry reset needs at least one child. None given!"
					);
			}

			mbr = start->mbr;
			node->nEntries = 0;

			for (;start != end; ++start) {
				add(*start);
			}
		};


		/**
		 * Remove all entries and add the given initializer list. This also
		 * fixes the MBR.
		 */
		Entry& operator=(std::initializer_list<Entry> entries)
		{
			assign(entries.begin(), entries.end());
			return *this;
		};


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
		Entry * begin()
		{
			return node->begin();
		};

		/**
		 * Return the entry past the last entry in this entry's node.
		 */
		Entry * end()
		{
			return node->end();
		};

};

}
