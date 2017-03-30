#pragma once
#include "Entry.hpp"
#include "HilbertCurve.hpp"
#include <cstdint>

namespace Rtree
{

/**
 * An entry in a node of the Hilbert R-tree.
 *
 * In addition to the regular entry fields, it also contain the largest hilbert
 * value seen by it's children and updates this value when necessary.
 *
 * @tparam N Node type
 */
template<unsigned D, class N>
class HilbertEntry : public BaseEntry<D, N, HilbertEntry>
{
	public:
		using M = Mbr<D>;
		using Node = N;
		using HC = HilbertCurve<std::uint64_t, D>;
		using Entry = BaseEntry<D, N, HilbertEntry>;

		// Inherit assignment operator
		using BaseEntry<D, N, HilbertEntry>::operator=;

		/**
		 * Default constructor.
		 */
		HilbertEntry() = default;

		/**
		 * Create a new entry from a node.
		 */
		HilbertEntry(N * node, M mbr) : Entry(node, mbr)
		{
			for (auto e : *this) {
				hilbertValue = std::max(hilbertValue, e.hilbertValue);
			}
		};

		/**
		 * Create a new entry and initialize MBR and node with the given
		 * entries.
		 *
		 * @param node Pointer to initial node
		 * @param entries Initial entries
		 */
		HilbertEntry(N * node, std::initializer_list<HilbertEntry> entries)
			: Entry(node, entries)
		{
		};


		/**
		 * Create a new entry from the given object.
		 */
		HilbertEntry(const DataObject& object, const Box& bounds)
			: Entry(object)
		{
			hilbertValue = HC::map(this->getMbr().center(), bounds);
		};


		/*
		 * Add an entry as a child of this entry.
		 * The entry will be added to the child node, and the MBR and
		 * hilbertValue updated.
		 *
		 * @param entry Entry to add
		 */
		void add(const HilbertEntry& entry)// override
		{
			Entry::add(entry);

			std::inplace_merge(
					this->begin(),
					this->end() - 1,
					this->end(),
					[](const HilbertEntry& a, const HilbertEntry& b) {
						return a.getHilbertValue() < b.getHilbertValue();
					}
				);

			hilbertValue = std::max(hilbertValue, entry.hilbertValue);
		};


		/**
		 * Update this entry to include the given one somewhere below in the
		 * hierarchy. This does not add the given entry, only updates relevant
		 * data about the children of this node.
		 *
		 * @param entry New entry to include
		 */
		void include(const HilbertEntry& entry)
		{
			this->getMbr() += entry.getMbr();
			hilbertValue = std::max(hilbertValue, entry.hilbertValue);
		}


		/**
		 * Reset this node to contain only the entries in the given range.
		 *
		 * @param start First element
		 * @param end Element beyond the last
		 */
		template<class ForwardIterator>
		void assign(ForwardIterator start, ForwardIterator end)
		{
			hilbertValue = 0;
			BaseEntry<D, N, HilbertEntry>::assign(start, end);
		};


		/**
		 * Get the largest hilbert value in this node.
		 */
		std::uint64_t getHilbertValue() const
		{
			return hilbertValue;
		};

	private:
		std::uint64_t hilbertValue;

};

}
