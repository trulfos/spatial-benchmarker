#pragma once
#include "Algorithm.hpp"
#include <vector>
#include <algorithm>
#include <iterator>

namespace Rtree
{

/**
 * Creates a view of the items in a range.
 *
 * Each entry in this view only refers to the original objects and moving the
 * original objects thus invalidates this view.
 *
 * @tparam E Item type
 */
template<class It>
class ReferenceView : private std::vector<It>
{
	using Base = std::vector<It>;
	using Base::push_back;

	public:
		using value_type = typename std::iterator_traits<It>::value_type;
		using iterator = DerefIterator<typename Base::iterator>;
		using const_iterator = DerefIterator<typename Base::const_iterator>;

		/**
		 * Create a new view with the given range of entries.
		 *
		 * Note that the entries must remain in their memory location while the
		 * ReferenceView is in use.
		 */
		ReferenceView(It first, It last);


		/**
		 * Default constructor.
		 *
		 * Gives an empty view.
		 */
		ReferenceView();


		/**
		 * Add elements to this view.
		 */
		void insert(It first, It last);


		/**
		 * Sort the entries in this view using the given compare.
		 */
		template<class Compare>
		void sort(Compare compare);

		/**
		 * Get the number of items in this container.
		 */
		using Base::size;


		/**
		 * Get a specific item by index.
		 */
		It operator[](size_t i);


		/**
		 * Start iteration through this views entries.
		 */
		iterator begin();
		const_iterator begin() const;


		/**
		 * Iterator to past the end.
		 */
		iterator end();
		const_iterator end() const;
};


/*
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
 */


template<class It>
ReferenceView<It>::ReferenceView(It first, It last)
{
	insert(first, last);
}


template<class It>
ReferenceView<It>::ReferenceView()
{
}


template<class It>
void ReferenceView<It>::insert(It first, It last)
{
	// Materialize
	while (first != last) {
		push_back(first);
		++first;
	}
}


template<class It>
template<class Compare>
void ReferenceView<It>::sort(Compare compare)
{
	std::sort(
			Base::begin(), Base::end(),
			[&](const It a, const It b) {
				return compare(*a, *b);
			}
		);
}

template<class It>
It ReferenceView<It>::operator[](size_t i)
{
	return Base::operator[](i);
}

template<class It>
typename ReferenceView<It>::iterator ReferenceView<It>::begin()
{
	return iterator(Base::begin());
}

template<class It>
typename ReferenceView<It>::iterator ReferenceView<It>::end()
{
	return iterator(Base::end());
}

template<class It>
typename ReferenceView<It>::const_iterator ReferenceView<It>::begin() const
{
	return const_iterator(Base::begin());
}

template<class It>
typename ReferenceView<It>::const_iterator ReferenceView<It>::end() const
{
	return const_iterator(Base::end());
}

}
