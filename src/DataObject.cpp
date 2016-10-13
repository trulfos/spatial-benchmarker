#include <stdexcept>

#include "DataObject.hpp"

DataObject::DataObject(unsigned int dimension)
	: dimension(dimension), point(dimension)
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
	stream >> object.id >> object.point;

	return stream;
}


std::ostream& operator<<(
		std::ostream& stream,
		const DataObject& object
) {
	return stream << "Data object " << object.id << std::endl
		<< "  " << object.point;
}
