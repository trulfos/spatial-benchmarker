#include "DataObject.hpp"
#include <ios>
#include <stdexcept>

namespace Spatial
{

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

}
