#pragma once
#include <string>
#include <fstream>
#include "common/ResultSet.hpp"
#include "LazyDataSet.hpp"
#include "LazyQuerySet.hpp"

/**
 * This class is responsible for providing the correct data as read from a
 * folder on the file system.
 */
class Benchmark
{
	public:
		/**
		 * Read benchmark from the given folder.
		 *
		 * The folder typically contains the following files.
		 *
		 *	Filename    | Description
		 *	--------------------------------------------------------------------
		 *	 dimension  |  Contains a single number - the dimension of the data
		 *	 data       |  Binary file with data objects
		 *	 queries    |  Query rectangles in binary format
		 *	 results	|  List of maching rectangles for each query (by id)
		 */
		Benchmark(const std::string& folder);

		/**
		 * Open and return data set file.
		 */
		LazyDataSet getData();

		/**
		 * Open and return query set file.
		 */
		LazyQuerySet getQueries();

		/**
		 * Read and return the result set.
		 *
		 * @param empty Return empty sets if true instead of reading from file
		 */
		ResultSet getResults(bool empty = false);

	private:

		std::string folder;
		unsigned dimension;
};
