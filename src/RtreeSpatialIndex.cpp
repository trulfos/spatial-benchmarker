#include "RtreeSpatialIndex.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath>

RtreeSpatialIndex::RtreeSpatialIndex(const DataSet& dataSet)
{
	if (dataSet.empty()) {
		return;
	}

	dimension = dataSet[0].getPoint().getDimension();

	// Allocate root node
	root =  new (dimension, maxChildren) Node();
	height = 1;
};

RtreeSpatialIndex::~RtreeSpatialIndex()
{
	//TODO: Traverse tree and delete it
};


ResultSet RtreeSpatialIndex::rangeSearch(const AxisAlignedBox&	 box) const
{
	ResultSet resultSet;

	return resultSet;
};

ResultSet RtreeSpatialIndex::knnSearch(unsigned k, const Point& point) const
{
	ResultSet results;

	return results;
};

void RtreeSpatialIndex::insert(const DataObject& object, Node * node)
{
	// Find leaf node
	for (unsigned i = 0; i < height - 1; i++) {

		// Find best child
		for (unsigned j = 0; j < node->nEntries; j++) {
		}
	}

	// node is now a leaf
	if (node->nEntries < maxChildren) {
		// Create entry
		*((Entry *) (((char *) node->entries) + node->nEntries * sizeof(Entry))) =
			Entry(object.getId());

		// Fill mbr
		
		node->nEntries++;
	}
}

RtreeSpatialIndex::Entry::Entry(DataObject::Id id) : id(id)
{
}

void * RtreeSpatialIndex::Node::operator new (size_t size, unsigned d, unsigned capacity)
{
	return ::operator new (
			size + capacity * (
				sizeof(Entry) + 2 * d * sizeof(float)
			) 
		);
}
