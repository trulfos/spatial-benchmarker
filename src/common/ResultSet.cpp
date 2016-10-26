#include "ResultSet.hpp"
#include "FileHeader.hpp"
#include <iostream>
#include <iterator>

std::istream& operator>>(std::istream& stream, ResultSet& resultSet)
{
	unsigned dimension;
	unsigned nResults;

	stream >> dimension >> nResults;

	resultSet.resize(nResults);
	for (unsigned i = 0; i < nResults; ++i) {
		stream >> resultSet[i];
	}

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const ResultSet& resultSet)
{
	stream << FileHeader(0, resultSet.size()) << "\n";

	for (auto& r : resultSet) {
		std::cout << r << "\n";
	}

	return stream;
}
