#include "Scanning.hpp"
#include <iostream>

namespace Scanning
{

Scanning::Scanning(unsigned dimension, unsigned long long size)
	: dimension(dimension)
{
	// Allocate buffers
	positions = new Coordinate[2 * size * dimension];
	ids = new DataObject::Id[size];
}

void Scanning::insert(const DataObject& object)
{
	unsigned& i = nObjects;

	ids[i] = object.getId();

	const auto& points = object.getBox().getPoints();
	for (unsigned j = 0; j < dimension; j++) {
		positions[2 * (dimension * i + j)] = points.first[j];
		positions[2 * (dimension * i + j) + 1] = points.second[j];
	}

	i++;
};

Scanning::~Scanning()
{
	delete[] positions;
	delete[] ids;
};

}
