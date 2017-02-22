#pragma once
#include <cassert>
#include <functional>
#include <type_traits>
#include <tuple>

#include <iostream>

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
class RangeIterator : public std::iterator<std::forward_iterator_tag, const T>
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

		const T& operator*() const
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


/**
 * Dereferencing iterator wrapper.
 *
 * This is useful when an iterator points to another iterator (or pointer). The
 * dereferencing operator dereferences twice to get the actual value pointed to.
 */
template<typename Iterator>
class DerefIterator : public std::iterator<std::forward_iterator_tag, typename Iterator::value_type>
{
	public:
		DerefIterator(const Iterator& iterator) : iterator(iterator)
		{
		};

		DerefIterator operator++()
		{
			++iterator;
			return *this;
		};

		decltype(*(typename Iterator::value_type()))& operator*()
		{
			return **iterator;
		};

		typename Iterator::value_type operator->()
		{
			return *iterator;
		};

		bool operator==(const DerefIterator& other) const
		{
			return iterator == other.iterator;
		};

		bool operator!=(const DerefIterator& other) const
		{
			return iterator != other.iterator;
		};

		typename Iterator::difference_type operator-(
				const DerefIterator& other
			) const
		{
			return iterator - other.iterator;
		};

	private:
		Iterator iterator;
};

template<typename Iterator>
DerefIterator<Iterator> makeDerefIt(const Iterator& iterator)
{
	return DerefIterator<Iterator>(iterator);
};


/**
 * Gives the combinations possible by combining a range with itself.
 */
template<class FIt>
class CombinationsView
{
	public:

		using value_type = std::pair<
				typename std::iterator_traits<FIt>::reference,
				typename std::iterator_traits<FIt>::reference
			>;

		class CombinationsIterator
			: public std::iterator<std::forward_iterator_tag, value_type>

		{
			public:
				CombinationsIterator(CombinationsView * view, FIt current)
					: first(current), second(current), view(view)
				{
					if (current != view->last) {
						operator++();
					}
				}

				CombinationsIterator operator++()
				{
					assert(
							first != view->last &&
							second != view->last
						);

					++second;

					if (second == view->last) {
						++first;
						second = first;
						++second;

						if (second == view->last) {
							++first;
						}
					}

					assert(
							(first == view->last) == (second == view->last)
						);

					return *this;
				}

				value_type operator*()
				{
					return value_type(*first, *second);
				}

				bool operator==(const CombinationsIterator& other) const
				{
					return view == other.view &&
						first == other.first &&
						second == other.second;
				}

				bool operator!=(const CombinationsIterator& other) const
				{
					return !(*this == other);
				}

				std::pair<FIt, FIt> getIterators()
				{
					return std::make_pair(first, second);
				}

			private:
				FIt first, second;
				CombinationsView * view;
		};

		using iterator = CombinationsIterator;

		CombinationsView(FIt first, FIt last) : first(first), last(last)
		{
		}

		iterator begin()
		{
			return CombinationsIterator(this, first);
		}

		iterator end()
		{
			return CombinationsIterator(this, last);
		}

	private:
		FIt first;
		FIt last;
};

template<class FIt>
CombinationsView<FIt> makeCombinationsView(FIt first, FIt last)
{
	return CombinationsView<FIt>(first, last);
}
