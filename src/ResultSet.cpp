#include "ResultSet.hpp"
#include "DataObject.hpp"

std::istream& operator>>(std::istream& stream, ResultSet& resultSet)
{
	unsigned n;

	stream >> n;

	for (unsigned i = 0; i < n; i++) {
		DataObject::Id id;
		stream >> id;
		resultSet.push_back(id);
	}

	return stream;
}
