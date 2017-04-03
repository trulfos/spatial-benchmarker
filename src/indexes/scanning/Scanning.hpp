#pragma once
#include "spatial/Coordinate.hpp"
#include "spatial/DataObject.hpp"
#include "spatial/SpatialIndex.hpp"

using namespace Spatial;

namespace Scanning
{

/**
 * Keeps an array of all data objects and ids so that a linear scan can be done
 * to generate search results.
 */
class Scanning : public ::SpatialIndex
{

	public:
		Scanning(unsigned dimension, unsigned long long size);
		virtual ~Scanning();

		// Cannot be copied
		Scanning(const Scanning&) = delete;

		void insert(const DataObject& object) override;

	protected:
		unsigned nObjects = 0;
		unsigned dimension;
		Coordinate * positions;
		DataObject::Id * ids;
};

}
