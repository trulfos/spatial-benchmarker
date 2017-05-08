#pragma once
#include "Algorithm.hpp"
#include "ReferenceView.hpp"
#include "Split.hpp"
#include <vector>
#include <algorithm>
#include <memory>
#include <iterator>

namespace Rtree
{

/**
 * A collection of possible splits.
 */
template<class N, unsigned m>
class SplitSet
{
	using NIt = typename N::iterator;

	public:
		static constexpr unsigned D = N::Mbr::dimension;

		using value_type = Split<N>;

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
						const std::vector<Entry<N>>& entries,
						unsigned dimension
					);

				/**
				 * The usual iterator operations
				 */
				bool operator==(const SplitIterator& other) const;
				bool operator!=(const SplitIterator& other) const;
				SplitIterator operator++();
				const Split<N>& operator*() const;
				const Split<N>* operator->() const;

			private:

				std::shared_ptr<Split<N>> split;
				std::vector<const Entry<N> *> entryView;
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
		std::vector<Entry<N>> entries;
};



/*
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
*/

template<class N, unsigned m>
template<class FIt>
SplitSet<N, m>::SplitSet(FIt first, FIt last)
{
	entries.insert(entries.end(), first, last);
}


template<class N, unsigned m>
template<class FIt>
SplitSet<N, m>::SplitSet(FIt first, FIt first2, FIt last, FIt last2)
{
	entries.insert(entries.end(), first, last);
	entries.insert(entries.end(), first2, last2);
}


template<class N, unsigned m>
typename SplitSet<N, m>::iterator SplitSet<N, m>::begin() const
{
	return iterator(
			entries,
			dimension == D ? 0 : dimension
		);
}


template<class N, unsigned m>
typename SplitSet<N, m>::iterator SplitSet<N, m>::end() const
{
	return iterator(
			entries,
			dimension == D ? D : dimension + 1
		);
}


template<class N, unsigned m>
void SplitSet<N, m>::restrictTo(unsigned dimension)
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
template<class N, unsigned m>
SplitSet<N, m>::SplitIterator::SplitIterator(
		const std::vector<Entry<N>>& entries,
		unsigned dimension
	) : candidate(2 * dimension * (entries.size() - 2 * m + 1))
{
	std::transform(
			entries.begin(), entries.end(),
			std::back_inserter(entryView),
			[](const Entry<N>& e) {
				return &e;
			}
		);


	sort();

	// Create first split
	split = std::make_shared<Split<N>>(
			this->entryView,
			getSortOrder(),
			getDimension(),
			getSplitPoint()
		);
}


template<class N, unsigned m>
bool SplitSet<N, m>::SplitIterator::operator==(
		const SplitSet<N, m>::SplitIterator& other
	) const
{
	return candidate == other.candidate;
}


template<class N, unsigned m>
bool SplitSet<N, m>::SplitIterator::operator!=(
		const SplitSet<N, m>::SplitIterator& other
	) const
{
	return !(*this == other);
}


/**
 * Increment to the next split.
 */
template<class N, unsigned m>
typename SplitSet<N, m>::SplitIterator
	SplitSet<N, m>::SplitIterator::operator++()
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
	split = std::make_shared<Split<N>>(
			entryView,
			sortOrder,
			dimension,
			getSplitPoint()
		);

	return *this;
}


template<class N, unsigned m>
const Split<N>& SplitSet<N, m>::SplitIterator::operator*() const
{
	return *split;
}


template<class N, unsigned m>
const Split<N>* SplitSet<N, m>::SplitIterator::operator->() const
{
	return split.get();
}


template<class N, unsigned m>
unsigned SplitSet<N, m>::SplitIterator::getSplitPoint() const
{
	return m + candidate % (entryView.size() - 2 * m + 1);
}


template<class N, unsigned m>
unsigned SplitSet<N, m>::SplitIterator::getSortOrder() const
{
	return (candidate / (entryView.size() - 2 * m + 1)) % 2;
}


template<class N, unsigned m>
unsigned SplitSet<N, m>::SplitIterator::getDimension() const
{
	return candidate / (2 * (entryView.size() - 2 * m + 1));
}


template<class N, unsigned m>
void SplitSet<N, m>::SplitIterator::sort()
{
	unsigned d = getDimension();

	if (d >= D) {
		return;
	}

	using E = typename std::iterator_traits<NIt>::value_type;

	if (getSortOrder() == 0) {
		std::sort(
				entryView.begin(), entryView.end(),
				[&](const E * a, const E * b) {
					return std::tie(
							a->getMbr().getTop()[d],
							a->getMbr().getBottom()[d]
						) < std::tie(
								b->getMbr().getTop()[d],
								b->getMbr().getBottom()[d]
							);
				}
			);
	} else {
		std::sort(
				entryView.begin(), entryView.end(),
				[&](const E * a, const E * b) {
					return std::tie(
							a->getMbr().getBottom()[d],
							a->getMbr().getTop()[d]
						) < std::tie(
								b->getMbr().getBottom()[d],
								b->getMbr().getTop()[d]
							);
				}
			);
	}
}

}
