#pragma once
#include <istream>
#include <string>
#include <cassert>
#include <fstream>
#include "common/DataObject.hpp"
#include "common/FileHeader.hpp"
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
						unsigned index,
						bool binary
					);

				bool operator!=(const LazyDataSetIterator& other);
				LazyDataSetIterator operator++();
				DataObject& operator*();

			private:
				std::istream& stream;
				DataObject element;
				bool extracted = false;
				unsigned index;
				bool binary;
		};


		/**
		 * Create a new lazy data set.
		 *
		 * Opens the given file with either a `.dat` extension or a `.csv`
		 * extension if the first cannot be opened. The `.dat` file is expected
		 * to be in binary format, while the `.csv` file is expeced to be plain
		 * text.
		 *
		 * @param filename File prefix
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


		/**
		 * Check whether the binary or text edition of the file was opened.
		 *
		 * @return True if the binary `.dat` file was opened
		 */
		bool isBinary() const;

	private:
		FileHeader header;
		std::fstream stream;
		bool iterated = false;
		bool binary = false;
};
