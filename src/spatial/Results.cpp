#include "Results.hpp"
#include "DataObject.hpp"
#include <iterator>

namespace Spatial
{

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


std::ostream& operator<<(std::ostream& stream, const Results& results)
{
	stream << results.size() << ' ';

	std::copy(
			results.begin(),
			results.end(),
			std::ostream_iterator<DataObject::Id>(stream, " ")
		);

	return stream;
}

}
