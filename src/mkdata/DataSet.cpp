#include "DataSet.hpp"
#include <iterator>


FileHeader DataSet::getFileHeader() const
{
	return FileHeader(
			getDimension(),
			size()
		);
}


unsigned DataSet::getDimension() const
{
	if (size() < 1) {
		throw std::logic_error(
				"Cannot get dimension without any data objects"
			);
	}

	return operator[](0).getPoint().getDimension();
}


void DataSet::write(std::ostream& stream) const
{
	getFileHeader().write(stream);

	for (const auto& object : *this) {
		object.write(stream);
	}
}


void DataSet::read(std::istream& stream)
{
	FileHeader fileHeader;
	fileHeader.read(stream);

	unsigned nObjects = fileHeader.getN();

	while (nObjects--) {
		emplace_back(fileHeader.getDimension());
		back().read(stream);
	}
}


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

	stream << dataSet.getFileHeader() << "\n";

	std::copy(
			dataSet.begin(),
			dataSet.end(),
			std::ostream_iterator<DataObject>(stream, "\n")
		);

	return stream;
}
