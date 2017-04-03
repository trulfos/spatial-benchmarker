#include "ResultSet.hpp"
#include "FileHeader.hpp"
#include <iostream>
#include <iterator>

namespace Bench
{

std::istream& operator>>(std::istream& stream, ResultSet& resultSet)
{
	unsigned dimension;
	unsigned nResults;

	stream >> dimension >> nResults;

	if (stream) {
		resultSet.resize(nResults);
		for (unsigned i = 0; i < nResults && stream; ++i) {
			Spatial::operator>>(stream, resultSet[i]);
		}
	}

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const ResultSet& resultSet)
{
	stream << FileHeader(0, resultSet.size()) << "\n";

	for (auto& r : resultSet) {
		Spatial::operator<<(std::cout, r);
	}

	return stream;
}

}
