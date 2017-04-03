#pragma once
#include <istream>
#include <ostream>
#include <vector>
#include "Box.hpp"

namespace Spatial
{

/**
 * This represents an data object.
 *
 * These objects are typically indexed and searched through in a spatial index,
 * but may also be used for other operations, e.g. as a query object for the
 * nearest neighbors.
 *
 * Currently, this is just a point in R^d.
 */
class DataObject
{
	public:
		using Id = unsigned long long;

		/**
		 * Create a new spatial object.
		 */
		DataObject(unsigned int dimension);
		DataObject(Id id, const Box& point);
		DataObject() = default;

		/**
		 * Get the id of this object.
		 *
		 * Returns 0 if this object has not yet been initialized.
		 */
		Id getId() const;

		/**
		 * Get the point associated with this data object.
		 *
		 * May return garbage if the point has not been initialized.
		 */
		const Box& getBox() const;


		/**
		 * Write this object to a stream in binary.
		 */
		void write(std::ostream& stream) const;

		/**
		 * Read this object from a binary stream.
		 */
		void read(std::istream& stream);

	private:
		Id id = 0;
		Box box;
};

}
