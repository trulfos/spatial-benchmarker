#pragma once
#include "Link.hpp"
#include "Mbr.hpp"
#include <iterator>

namespace Rtree
{

	/**
	 * Node where the pointers are stored as a separate array.
	 *
	 * @tparam D Dimension of MBRs
	 * @tparam C Maximum node capacity
	 */
	template<unsigned D, unsigned C>
	class PointerArrayNode
	{
		public:

			using L = Link<PointerArrayNode>;

			/**
			 * Iterator running through links in this node matching a query.
			 */
			class ScanIterator
				: public std::iterator<std::forward_iterator_tag, const L>
			{
				using Base = std::iterator<std::forward_iterator_tag, const L>;

				public:
					using reference = typename Base::reference;
					using pointer = typename Base::pointer;

					/**
					 * Construct a singular iterator.
					 */
					ScanIterator();

					/**
					 * Construct a scan iterator yielding entries overlapping
					 * MBR.
					 *
					 * Note that only a reference to the given MBR is store,
					 * thus it must be available during the lifetime of this
					 * iterator.
					 *
					 * @param node Node to scan through
					 * @param mbr MBR yielded entries must overlap
					 * @param i Index to start at
					 */
					ScanIterator(
							const PointerArrayNode * node,
							const Mbr<D>* mbr,
							unsigned i = 0
						);

					/**
					 * Advance to next matching entry.
					 *
					 * This scans through the node until the end is reached or a
					 * new entry overlapping with the MBR is found.
					 *
					 * @return This scan iterator
					 */
					ScanIterator& operator++();


					/**
					 * Advance this to the next matching entry.
					 *
					 * Scan through the node until a matching entry is found and
					 * then return the previous value of this.
					 *
					 * @return Previous scan iterator
					 */
					ScanIterator operator++(int);


					/**
					 * Check if this iterator is equal with another.
					 *
					 * This is the case if they are at the same position in the
					 * scan.
					 *
					 * @return True if equal
					 */
					bool operator==(const ScanIterator& other);

					/**
					 * Check if two iterators are unequal.
					 *
					 * The opposite of the equal operation, as expected.
					 *
					 * @return True if they are unequal
					 */
					bool operator!=(const ScanIterator& other);

					/**
					 * Retreive the link pointed to by this iterator.
					 *
					 * @return Reference to link
					 */
					reference operator*();

					/**
					 * Access the link pointed to by this iterator.
					 */
					pointer operator->();

				private:
					const PointerArrayNode * node;
					const Mbr<D> * mbr;
					unsigned i;

					/**
					 * Finds the first position intersecting with the MBR.
					 *
					 * This includes the current position if it intersects with
					 * the MBR. `i` is updated to the found position.
					 */
					void findNext();
			};

			using iterator = ScanIterator;


			/**
			 * Add an entry to this node.
			 *
			 * The entry may be returned during search.
			 *
			 * @param mbr MBR of entry
			 * @param link Link to child node or an object id
			 */
			void add(const Mbr<D>& mbr, const L& link);


			/**
			 * Create an iterator over overlapping entries.
			 *
			 * The iterator returns the link of entries overlapping with the
			 * box. When no more such entries exist, the iterator is equal to
			 * the past-the-end iterator.
			 */
			ScanIterator scan(const Mbr<D>& mbr) const;


			/**
			 * Return a past-the-end iterator, as expected.
			 */
			ScanIterator end() const; 


			/**
			 * Get a vector of all links in this node.
			 *
			 * @return Vector of links
			 */
			std::vector<Link<PointerArrayNode>> getLinks();

		private:
			Mbr<D> mbrs[C];
			Link<PointerArrayNode> links[C];
			unsigned nEntries = 0;

	};


	template<unsigned D, unsigned C>
	PointerArrayNode<D, C>::ScanIterator::ScanIterator()
		:node(nullptr)
	{
	}

	template<unsigned D, unsigned C>
	PointerArrayNode<D, C>::ScanIterator::ScanIterator(
			const PointerArrayNode * node,
			const Mbr<D>* mbr,
			unsigned i
		) : node(node), mbr(mbr), i(i)
	{
		findNext();
	}

	template<unsigned D, unsigned C>
	typename PointerArrayNode<D, C>::ScanIterator&
		PointerArrayNode<D, C>::ScanIterator::operator++()
	{
		++i;
		findNext();
		return *this;
	}


	template<unsigned D, unsigned C>
	typename PointerArrayNode<D, C>::ScanIterator
		PointerArrayNode<D, C>::ScanIterator::operator++(int)
	{
		ScanIterator it = *this;
		operator++();
		return it;
	}


	template<unsigned D, unsigned C>
	bool PointerArrayNode<D, C>::ScanIterator::operator==(
			const ScanIterator& other
		)
	{
		assert(other.node == node);
		return i == other.i;
	}

	template<unsigned D, unsigned C>
	bool PointerArrayNode<D, C>::ScanIterator::operator!=(
			const ScanIterator& other
		)
	{
		return !(*this == other);
	}

	template<unsigned D, unsigned C>
	typename PointerArrayNode<D, C>::ScanIterator::reference
		PointerArrayNode<D, C>::ScanIterator::operator*()
	{
		assert(i < node->nEntries);
		return node->links[i];
	}

	template<unsigned D, unsigned C>
	typename PointerArrayNode<D, C>::ScanIterator::pointer
		PointerArrayNode<D, C>::ScanIterator::operator->()
	{
		assert(i < node->nEntries);
		return &(node->links[i]);
	}


	template<unsigned D, unsigned C>
	void PointerArrayNode<D, C>::ScanIterator::findNext()
	{
		while (i < node->nEntries && !node->mbrs[i].intersects(*mbr))
		{
			++i;
		}
	}



	template<unsigned D, unsigned C>
	void PointerArrayNode<D, C>::add(const Mbr<D>& mbr, const L& link)
	{
		assert(nEntries < C);

		mbrs[nEntries] = mbr;
		links[nEntries] = link;

		++nEntries;
	}


	template<unsigned D, unsigned C>
	typename PointerArrayNode<D, C>::ScanIterator PointerArrayNode<D, C>::scan(
			const Mbr<D>& mbr
		) const
	{
		return ScanIterator(this, &mbr);
	}


	template<unsigned D, unsigned C>
	typename PointerArrayNode<D, C>::ScanIterator PointerArrayNode<D, C>::end()
		const
	{
		return ScanIterator(this, nullptr, nEntries);
	}


	template<unsigned D, unsigned C>
	std::vector<Link<PointerArrayNode<D, C>>> PointerArrayNode<D, C>::getLinks()
	{
		return std::vector<Link<PointerArrayNode<D, C>>>(
				links,
				links + nEntries
			);
	}

}
