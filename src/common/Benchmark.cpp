#include "Benchmark.hpp"

#include "AxisAlignedBox.hpp"
#include "DataObject.hpp"
#include "KnnQuery.hpp"
#include "RangeQuery.hpp"
#include <ios>

Benchmark::~Benchmark()
{
	for (auto query : queries) {
		delete query;
	}
}


unsigned Benchmark::getDimension()
{
	return dimension;
}


const DataSet& Benchmark::getDataSet()
{
	return dataSet;
}


std::vector<TestCase> Benchmark::getTestCases()
{
	unsigned n = queries.size();
	std::vector<TestCase> testCases (n);

	for (unsigned i = 0; i < n; i++) {
		testCases[i] = TestCase(queries[i], &results[i]);
	}

	return testCases;
}


/**
 * Expected format:
 *
 * <dimension> <num_objects> <num_queries>
 * <object>
 * ...
 * <object>
 * <query> <results>
 * ...
 * <query> <results>
 */
std::istream& operator>>(std::istream& stream, Benchmark& benchmark)
{
	unsigned nObjects, nQueries;
	unsigned& dimension = benchmark.dimension;

	stream >> dimension >> nObjects >> nQueries;

	// Read objects
	for (unsigned i = 0; i < nObjects; i++) {
		DataObject object (dimension);
		stream >> object;
		benchmark.dataSet.push_back(object);
	}

	auto& results = benchmark.results;
	auto& queries = benchmark.queries;

	results.resize(nQueries);
	queries.resize(nQueries);

	// Read queries
	for (unsigned i = 0; i < nQueries; i++) {
		std::string type;
		stream >> type;

		if (type == "knn") {
			unsigned k;
			Point p (dimension);
			stream >> k >> p;
			queries[i] = new KnnQuery(k, p);
		} else if (type == "range") {
			Point p1 (dimension), p2 (dimension);
			stream >> p1 >> p2;
			queries[i] = new RangeQuery(AxisAlignedBox(p1, p2));
		} else {
			stream.setstate(std::ios_base::failbit);
			return stream;
		}
	}

	// Read results
	for (unsigned i = 0; i < nQueries; i++) {
		stream >> results[i];
	}

	return stream;
}
