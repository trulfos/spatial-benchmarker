#include "Results.hpp"
#include "DataObject.hpp"

std::istream& operator>>(std::istream& stream, Results& results)
{
	unsigned n;
	stream >> n;

	for (unsigned i = 0; i < n; i++) {
		DataObject::Id id;
		stream >> id;
		results.push_back(id);
	}

	return stream;
}
