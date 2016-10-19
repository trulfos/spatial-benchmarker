#pragma once
#include "DataSet.hpp"
#include "Query.hpp"
#include "ResultSet.hpp"
#include <istream>
#include <ostream>
#include <vector>

typedef std::pair<const Query *, const ResultSet *> TestCase;

/**
 * A benchmark consists of a test data set, a set of queries and their expected
 * results.
 */
class Benchmark
{
	public:
		~Benchmark();

		unsigned getDimension();
		const DataSet& getDataSet();

		std::vector<TestCase> getTestCases();

	private:
		unsigned dimension;
		DataSet dataSet;
		std::vector<Query *> queries;
		std::vector<ResultSet> results;

		friend std::ostream& operator<<(
				std::ostream& stream,
				const Benchmark& benchmark
			);

		friend std::istream& operator>>(
				std::istream& stream,
				Benchmark& benchmark
			);

};
