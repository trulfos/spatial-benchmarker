#include "DataObject.hpp"
#include <ios>
#include <stdexcept>

DataObject::DataObject(unsigned int dimension) : box(dimension)
{
}


DataObject::DataObject(Id id, const Box& box) : id(id), box(box)
{
}

DataObject::Id DataObject::getId() const
{
	return id;
}

const Box& DataObject::getBox() const
{
	return box;
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

	stream >> object.box;

	return stream;
}


std::ostream& operator<<(
		std::ostream& stream,
		const DataObject& object
) {
	return stream << object.id << ' ' << object.box;
}
