#pragma once
#include "BoxInputIterator.hpp"
#include "spatial/DataObject.hpp"

/**
 * Reads data objects from input stream assuming a binary encoding.
 *
 * The usage is similar to std::istream_iterator. Check documentation for
 * examples.
 */
class DataObjectInputIterator : public BoxInputIterator
{
	public:
		using value_type = const DataObject;

		/**
		 * Creates an input iterator pointing to the start of the stream.
		 *
		 * @param stream Stream to read data from
		 * @param dimension Dimension to assume
		 */
		DataObjectInputIterator(std::istream& stream, unsigned dimension);

		DataObjectInputIterator() = default;


		// Override...
		const DataObject& operator*() const;
		const DataObject * operator->() const;
		DataObjectInputIterator operator++();
		DataObjectInputIterator operator++(int);
		difference_type operator-(DataObjectInputIterator other);


		/**
		 * This can be used as-is?
		 */
		//difference_type operator-(BoxInputIterator& other);

	private:
		unsigned id = 1;
		DataObject object;

		void extract();
};
