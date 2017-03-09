#pragma once
#include "Algorithm.hpp"
#include "ReferenceView.hpp"
#include "Split.hpp"
#include <vector>
#include <algorithm>
#include <memory>
#include <iterator>

/**
 * A collection of possible splits.
 */
template<class E, unsigned m>
class SplitSet
{
	public:
		static constexpr unsigned D = E::dimension;

		using value_type = Split<E>;

		/**
		 * Iterator for iterating through a split set.
		 */
		class SplitIterator
			: public std::iterator<std::input_iterator_tag, value_type>
		{
			public:
				/**
				 * Create an iterator starting at the given dimension.
				 */
				SplitIterator(
						const ReferenceView<E> entryView,
						unsigned dimension
					);

				/**
				 * The usual iterator operations
				 */
				bool operator==(const SplitIterator& other) const;
				bool operator!=(const SplitIterator& other) const;
				SplitIterator operator++();
				const Split<E>& operator*() const;
				const Split<E>* operator->() const;

			private:

				std::shared_ptr<Split<E>> split;
				ReferenceView<E> entryView;
				unsigned candidate;

				unsigned getSplitPoint() const;
				unsigned getSortOrder() const;
				unsigned getDimension() const;

				/**
				 * Sort the entries in this iterator.
				 */
				void sort();

		};

		using iterator = SplitIterator;

		template<class FIt>
		SplitSet(FIt first, FIt last);

		/**
		 * Construct a set of splits from the entries in two ranges.
		 */
		template<class FIt>
		SplitSet(FIt first, FIt first2, FIt last, FIt last2);


		/**
		 * Create an empty split set.
		 */
		SplitSet();


		/**
		 * Start iteration over all splits in this set.
		 */
		iterator begin() const;


		/**
		 * Past-the-end iterator for this set.
		 */
		iterator end() const;


		/**
		 * Restrict to only the given dimension.
		 * Only splits across the given dimension will be considered.
		 *
		 * @param dimension Dimension to consider
		 */
		void restrictTo(unsigned dimension);

	private:
		unsigned dimension = D;
		ReferenceView<E> entryView;
};



/*
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
*/

template<class E, unsigned m>
template<class FIt>
SplitSet<E, m>::SplitSet(FIt first, FIt last)
{
	entryView.insert(first, last);
}


template<class E, unsigned m>
template<class FIt>
SplitSet<E, m>::SplitSet(FIt first, FIt first2, FIt last, FIt last2)
{
	entryView.insert(first, last);
	entryView.insert(first2, last2);
}


template<class E, unsigned m>
typename SplitSet<E, m>::iterator SplitSet<E, m>::begin() const
{
	return iterator(
			entryView,
			dimension == D ? 0 : dimension
		);
}


template<class E, unsigned m>
typename SplitSet<E, m>::iterator SplitSet<E, m>::end() const
{
	return iterator(
			entryView,
			dimension == D ? D : dimension + 1
		);
}


template<class E, unsigned m>
void SplitSet<E, m>::restrictTo(unsigned dimension)
{
	assert(dimension < D);
	this->dimension = dimension;
}



/*

 ___      _ _ _   ___ _                _           
/ __|_ __| (_) |_|_ _| |_ ___ _ _ __ _| |_ ___ _ _ 
\__ \ '_ \ | |  _|| ||  _/ -_) '_/ _` |  _/ _ \ '_|
|___/ .__/_|_|\__|___|\__\___|_| \__,_|\__\___/_|  
    |_|                                            
*/
template<class E, unsigned m>
SplitSet<E, m>::SplitIterator::SplitIterator(
		const ReferenceView<E> entryView,
		unsigned dimension
	) : entryView(entryView),
		candidate(2 * dimension * (entryView.size() - 2 * m + 1))
{
	sort();

	// Create first split
	split = std::make_shared<Split<E>>(
			this->entryView,
			getSortOrder(),
			getDimension(),
			getSplitPoint()
		);
}


template<class E, unsigned m>
bool SplitSet<E, m>::SplitIterator::operator==(
		const SplitSet<E, m>::SplitIterator& other
	) const
{
	return candidate == other.candidate;
}


template<class E, unsigned m>
bool SplitSet<E, m>::SplitIterator::operator!=(
		const SplitSet<E, m>::SplitIterator& other
	) const
{
	return !(*this == other);
}


/**
 * Increment to the next split.
 */
template<class E, unsigned m>
typename SplitSet<E, m>::SplitIterator
	SplitSet<E, m>::SplitIterator::operator++()
{
	candidate++;

	// Bounds check
	assert(candidate <= (entryView.size() - 2 * m + 1) * 2 * D);

	unsigned sortOrder = getSortOrder();
	unsigned dimension = getDimension();

	// Do we have to sort?
	if (
			dimension != split->getDimension() ||
			sortOrder != split->getSort()
	) {
		sort();
	}

	// Create new split
	split = std::make_shared<Split<E>>(
			entryView,
			sortOrder,
			dimension,
			getSplitPoint()
		);

	return *this;
}


template<class E, unsigned m>
const Split<E>& SplitSet<E, m>::SplitIterator::operator*() const
{
	return *split;
}


template<class E, unsigned m>
const Split<E>* SplitSet<E, m>::SplitIterator::operator->() const
{
	return split.get();
}


template<class E, unsigned m>
unsigned SplitSet<E, m>::SplitIterator::getSplitPoint() const
{
	return m + candidate % (entryView.size() - 2 * m + 1);
}


template<class E, unsigned m>
unsigned SplitSet<E, m>::SplitIterator::getSortOrder() const
{
	return (candidate / (entryView.size() - 2 * m + 1)) % 2;
}


template<class E, unsigned m>
unsigned SplitSet<E, m>::SplitIterator::getDimension() const
{
	return candidate / (2 * (entryView.size() - 2 * m + 1));
}


template<class E, unsigned m>
void SplitSet<E, m>::SplitIterator::sort()
{
	unsigned d = getDimension();

	if (d >= D) {
		return;
	}

	if (getSortOrder() == 0) {
		entryView.sort([&](const E& a, const E& b) {
				return std::tie(
						a.mbr.getTop()[d],
						a.mbr.getBottom()[d]
					) < std::tie(
							b.mbr.getTop()[d],
							b.mbr.getBottom()[d]
						);
			});
	} else {
		entryView.sort([&](const E& a, const E& b) {
				return std::tie(
						a.mbr.getBottom()[d],
						a.mbr.getTop()[d]
					) < std::tie(
							b.mbr.getBottom()[d],
							b.mbr.getTop()[d]
						);
			});
	}
}
