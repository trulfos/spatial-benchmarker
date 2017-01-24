#pragma once
#include <cassert>
#include <functional>
#include <type_traits>

/**
 * Set of algorithm templates.
 */

/**
 * Does what it says on the tin. Given a transformation, it return the element
 * in the range which gives the smallest transformed value as determined by the
 * compare.
 */
template<class ForwardIt, class TransformFunc>
ForwardIt  argmin(ForwardIt start, ForwardIt end, TransformFunc transform)
{
	// No range deserves no answer
	if (start == end) {
		return end;
	}

	// Initial values from the fist element
	auto min = transform(*start);
	ForwardIt min_element = start;

	++start;

	// Update if we find something smaller
	while (start != end) {
		auto transformed = transform(*start);

		if (transformed < min) {
			min = transformed;
			min_element = start;
		}

		++start;
	}

	return min_element;
}


/**
 * Finds the minimum value after a tranform.
 */
template<class ForwardIt, class TransformFunc>
typename std::result_of<TransformFunc(typename ForwardIt::value_type)>::type
min_value(ForwardIt start, ForwardIt end, TransformFunc transform)
{
	assert(start != end);

	// Initial values from the fist element
	auto min = transform(*start);
	ForwardIt min_element = start;

	++start;

	// Update if we find something smaller
	while (start != end) {
		auto transformed = transform(*start);

		if (transformed < min) {
			min = transformed;
			min_element = start;
		}

		++start;
	}

	return min;
}



/**
 * Simple wrapper allowing the use of integers (and other similar datatypes) as
 * iterators.
 */
template<typename T>
class RangeIterator : public std::iterator<std::forward_iterator_tag, T>
{
	public:
		RangeIterator(const T& value) : value(value)
		{
		};

		RangeIterator operator++()
		{
			value++;
			return *this;
		};

		const T& operator*()
		{
			return value;
		};

		bool operator==(const RangeIterator& other) const
		{
			return value == other.value;
		};

		bool operator!=(const RangeIterator& other) const
		{
			return !(*this == other);
		};


	private:
		T value;
};

template<typename T>
RangeIterator<T> makeRangeIt(T value)
{
	return RangeIterator<T>(value);
};
