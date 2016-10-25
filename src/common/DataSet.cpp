#include "DataSet.hpp"

std::istream& operator>>(std::istream& stream, DataSet& dataSet)
{
	unsigned dimension;
	unsigned nObject;

	stream >> dimension >> nObject;

	while (nObject--) {
		dataSet.emplace_back(dimension);
		stream >> dataSet.back();
	}
}
