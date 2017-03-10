#pragma once
#include <iterator>


/**
 * Simple wrapper allowing the use of integers (and other similar datatypes) as
 * iterators.
 */
template<typename T>
class RangeIterator : public std::iterator<std::forward_iterator_tag, const T>
{
	public:
		RangeIterator(const T& value);

		RangeIterator operator++();
		const T& operator*() const;
		bool operator==(const RangeIterator& other) const;
		bool operator!=(const RangeIterator& other) const;

	private:
		T value;
};

template<typename T>
RangeIterator<T> makeRangeIt(T value)
{
	return RangeIterator<T>(value);
};


/**
 * Even simpler class representing a range.
 */
template<typename T>
class Range
{
	public:
		using iterator = RangeIterator<const T>;

		Range(T first, T last);

		iterator begin() const;
		iterator end() const;

	private:
		const T first, last;
};


template<typename T>
Range<T> makeRange(T first, T last)
{
	return Range<T>(first, last);
}


/*
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
*/

template<typename T>
RangeIterator<T>::RangeIterator(const T& value) : value(value)
{
};

template<typename T>
RangeIterator<T> RangeIterator<T>::operator++()
{
	value++;
	return *this;
};

template<typename T>
const T& RangeIterator<T>::operator*() const
{
	return value;
};

template<typename T>
bool RangeIterator<T>::operator==(const RangeIterator& other) const
{
	return value == other.value;
};

template<typename T>
bool RangeIterator<T>::operator!=(const RangeIterator& other) const
{
	return !(*this == other);
};

template<typename T>
Range<T>::Range(T first, T last) : first(first), last(last)
{
}

template<typename T>
typename Range<T>::iterator Range<T>::begin() const
{
	return iterator(first);
}

template<typename T>
typename Range<T>::iterator Range<T>::end() const
{
	return iterator(last);
}
