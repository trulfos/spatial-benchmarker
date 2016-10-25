#include "ResultSet.hpp"
#include <iostream>

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
