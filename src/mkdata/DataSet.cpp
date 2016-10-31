#include "DataSet.hpp"
#include "../common/FileHeader.hpp"
#include <iterator>

std::istream& operator>>(std::istream& stream, DataSet& dataSet)
{
	unsigned dimension;
	unsigned nObject;

	stream >> dimension >> nObject;

	while (nObject--) {
		dataSet.emplace_back(dimension);
		stream >> dataSet.back();
	}

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const DataSet& dataSet)
{
	if (dataSet.empty()) {
		return stream << 0u << ' ' << 0u;
	}

	stream << FileHeader(
			dataSet[0].getPoint().getDimension(),
			dataSet.size()
		) << std::endl;

	std::copy(
			dataSet.begin(),
			dataSet.end(),
			std::ostream_iterator<DataObject>(stream, "\n")
		);

	return stream;
}
