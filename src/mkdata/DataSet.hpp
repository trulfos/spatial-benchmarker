#pragma once

#include "common/DataObject.hpp"
#include "common/FileHeader.hpp"
#include <istream>
#include <vector>

/**
 * This represents a data set, consisting of data objects.
 */
class DataSet : public std::vector<DataObject>
{

	public:
		/**
		 * Generate a file header for this data set
		 */
		FileHeader getFileHeader() const;


		/**
		 * Get the dimension of this data set.
		 *
		 * WARNING: This is not verified and currently only checks the first
		 * item in the set.
		 */
		unsigned getDimension() const;


		/**
		 * Writes this data set in binary to the given ostream.
		 */
		void write(std::ostream& stream) const;

		/**
		 * Read this data set from a stream in binary.
		 */
		void read(std::istream& stream);
};


std::istream& operator>>(std::istream& stream, DataSet& dataSet);
std::ostream& operator<<(std::ostream& stream, const DataSet& dataSet);
