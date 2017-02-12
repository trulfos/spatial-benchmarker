#pragma once
#include <string>
#include <fstream>

/**
 * Represents a set of items stored in a file.
 *
 * Note that this only allows a single pass through the file.
 *
 * @tparam Iterator Iterator class constructible by a stream and dimension
 */
template<class Iterator>
class FileSet
{
	public:
		using value_type = typename Iterator::value_type;
		using iterator = Iterator;

		FileSet(const std::string& filename, unsigned dimension)
			: file(filename, std::ifstream::in | std::ifstream::binary)
		{
			if (!file) {
				throw std::runtime_error("Cannot read file " + filename);
			}

			start = Iterator(file, dimension);
		};

		FileSet() = default;


		/**
		 * Get iterator to current position in file.
		 */
		Iterator& begin()
		{
			return start;
		}

		/**
		 * Get singular iterator.
		 * This can be used as the end of the file.
		 */
		Iterator& end()
		{
			return stop;
		}


		/**
		 * Find the remaining size of the file.
		 *
		 * @return Number of elements left in file
		 */
		unsigned getSize()
		{
			return end() - begin();
		}


		/**
		 * Returns the dimensionality of the iterator.
		 */
		unsigned getDimension()
		{
			return start.getDimension();
		}

	private:
		std::ifstream file;
		Iterator start;
		Iterator stop;
};
