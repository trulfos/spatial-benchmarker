#include "DataObject.hpp"
#include <ios>
#include <stdexcept>

DataObject::DataObject(unsigned int dimension) : point(dimension)
{
}


DataObject::Id DataObject::getId() const
{
	return this->id;
}

const Point& DataObject::getPoint() const
{
	return this->point;
}


std::istream& operator>>(
		std::istream& stream,
		DataObject& object
) {
	stream >> object.id;

	if (object.id == 0) {
		stream.setstate(std::ios_base::failbit);
		return stream;
	}

	stream >> object.point;

	return stream;
}


std::ostream& operator<<(
		std::ostream& stream,
		const DataObject& object
) {
	return stream << "Data object " << object.id << std::endl
		<< "  " << object.point;
}
