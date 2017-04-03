#pragma once
#include "BoxInputIterator.hpp"
#include "spatial/Query.hpp"
#include "spatial/RangeQuery.hpp"

/**
 * Reads queries from input stream assuming a binary encoding.
 *
 * The usage is similar to std::istream_iterator. Check documentation for
 * examples.
 */
class QueryInputIterator : public BoxInputIterator
{
	public:
		using value_type = const RangeQuery;

		/**
		 * Creates an input iterator pointing to the start of the stream.
		 *
		 * @param stream Stream to read data from
		 * @param dimension Dimension to assume
		 */
		QueryInputIterator(std::istream& stream, unsigned dimension);

		QueryInputIterator() = default;


		// "Override"...
		const RangeQuery& operator*() const;
		const RangeQuery * operator->() const;
		QueryInputIterator operator++();
		QueryInputIterator operator++(int);
		difference_type operator-(QueryInputIterator other);

	private:
		unsigned id = 1;
		RangeQuery query;

		void extract();
};
