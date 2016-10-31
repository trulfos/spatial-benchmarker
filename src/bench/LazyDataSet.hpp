#pragma once
#include <istream>
#include <string>
#include <cassert>
#include <fstream>
#include "../common/DataObject.hpp"
#include "../common/FileHeader.hpp"
#include "LazyDataSet.hpp"

/**
 * Lazy query set.
 * Reads data from file as iteration progresses and does not store the data.
 * This avoids filling the memory with copies of the data while indexing.
 */
class LazyDataSet
{
	public:

		/**
		 * Iterator for iterating through this data set.
		 */
		class LazyDataSetIterator
				: public std::iterator<std::input_iterator_tag, DataObject>
		{
			public:
				LazyDataSetIterator(
						std::istream& stream,
						unsigned dimension,
						unsigned index
					);

				bool operator!=(const LazyDataSetIterator& other);
				LazyDataSetIterator operator++();
				DataObject& operator*();

			private:
				std::istream& stream;
				DataObject element;
				bool extracted = false;
				unsigned index;
		};


		/**
		 * Create a new lazy data set.
		 */
		LazyDataSet(std::string filename);


		/**
		 * Start iteration.
		 */
		LazyDataSetIterator begin();


		/**
		 * Iterator at the end.
		 */
		LazyDataSetIterator end();


		/**
		 * Get size of data set.
		 */
		unsigned size() const;


		/**
		 * Get data dimension.
		 */
		unsigned dimension() const;


		/**
		 * Check whether this data set is empty.
		 */
		bool empty() const;

	private:
		FileHeader header;
		std::fstream stream;
		bool iterated = false;
};
