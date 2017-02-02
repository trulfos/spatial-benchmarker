#pragma once
#include <iterator>
#include "common/Box.hpp"

/**
 * Reads boxes from input stream assuming a binary encoding.
 *
 * The usage is similar to std::istream_iterator. Check documentation for
 * examples.
 */
class BoxInputIterator : public std::iterator<std::input_iterator_tag, Box>
{
	public:

		/**
		 * Creates an input iterator pointing to the start of the stream.
		 *
		 * @param stream Stream to read data from
		 * @param dimension Dimension to assume
		 */
		BoxInputIterator(std::istream& stream, unsigned dimension);

		/**
		 * Creates an end iterator comparing equal to a depleted
		 * BoxInputIterator.
		 */
		BoxInputIterator();


		/**
		 * Standard iterator interface required to be an input iterator.
		 */
		bool operator==(const BoxInputIterator& other) const;
		bool operator!=(const BoxInputIterator& other) const;
		const Box& operator*() const;
		const Box * operator->() const;
		BoxInputIterator operator++();
		BoxInputIterator operator++(int);

		/**
		 * Compute the distance between two iterators.
		 *
		 * This can be used to determine the number of items left in the current
		 * stream. Note that this will require seeking through the stream.
		 */
		difference_type operator-(BoxInputIterator& other);


		/**
		 * Get the dimension of boxes from this iterator.
		 */
		unsigned getDimension() const;

	private:
		std::istream * stream;
		Box box;

		/**
		 * Get the size of a stream.
		 *
		 * This is measured by seeking to the end of the stream, telling the
		 * position, seeking back and calculating the difference.
		 */
		unsigned getStreamSize(std::istream& stream);
};
