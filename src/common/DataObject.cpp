#include "DataObject.hpp"
#include <ios>
#include <stdexcept>

DataObject::DataObject(unsigned int dimension) : point(dimension)
{
}


DataObject::DataObject(Id id, Point point) : id(id), point(point)
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


void DataObject::write(std::ostream& stream) const
{
	stream.write(reinterpret_cast<const char *>(&id), sizeof(id));
	point.write(stream);
}

void DataObject::read(std::istream& stream)
{
	stream.read(reinterpret_cast<char *>(&id), sizeof(id));
	point.read(stream);
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
	return stream << object.id << ' ' << object.point;
}
