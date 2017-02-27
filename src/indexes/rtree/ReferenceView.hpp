#pragma once
#include "common/Algorithm.hpp"
#include <vector>
#include <algorithm>

/**
 * Creates a view of the items in a range.
 *
 * Each entry in this view only refers to the original objects and moving the
 * original objects thus invalidates this view.
 *
 * @tparam E Item type
 */
template<class E>
class ReferenceView : private std::vector<E *>
{
	using Base = std::vector<E *>;

	public:
		using value_type = E;
		using iterator = DerefIterator<typename Base::iterator>;

		/**
		 * Create a new view with the given range of entries.
		 *
		 * Note that the entries must remain in their memory location while the
		 * ReferenceView is in use.
		 */
		template<class FIt>
		ReferenceView(FIt first, FIt last);


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
		E& operator[](size_t i);


		/**
		 * Start iteration through this views entries.
		 */
		iterator begin();


		/**
		 * Iterator to past the end.
		 */
		iterator end();
};


/*
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
 */


template<class E>
template<class FIt>
ReferenceView<E>::ReferenceView(FIt first, FIt last)
{
	std::transform(
			first, last,
			std::back_inserter(*static_cast<Base *>(this)),
			[](E& entry) {
				return &entry;
			}
		);
}

template<class E>
template<class Compare>
void ReferenceView<E>::sort(Compare compare)
{
	std::sort(
			Base::begin(), Base::end(),
			[&](const E * a, const E * b) {
				return compare(*a, *b);
			}
		);
}

template<class E>
E& ReferenceView<E>::operator[](size_t i)
{
	return *Base::operator[](i);
}

template<class E>
typename ReferenceView<E>::iterator ReferenceView<E>::begin()
{
	return makeDerefIt(Base::begin());
}

template<class E>
typename ReferenceView<E>::iterator ReferenceView<E>::end()
{
	return makeDerefIt(Base::end());
}
