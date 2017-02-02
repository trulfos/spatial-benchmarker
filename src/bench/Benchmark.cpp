#include "Benchmark.hpp"
#include <fstream>
#include <stdexcept>

Benchmark::Benchmark(const std::string& folder)
	: folder(folder)
{
	// Read dimension
	std::string path = folder + "dimension";
	std::ifstream dimensionFile(path);

	if (!dimensionFile || !(dimensionFile >> dimension)) {
		throw std::runtime_error("Cannot read file " + path);
	}
}


LazyDataSet Benchmark::getData()
{
	std::string path = folder + "data";
	return LazyDataSet(path, dimension);
}

LazyQuerySet Benchmark::getQueries()
{
	std::string path = folder + "queries";
	return LazyQuerySet(path, dimension);
}
