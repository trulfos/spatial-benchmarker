#pragma once

#include <istream>
#include <vector>
#include "Point.hpp"

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
		DataObject(Id id, Point point);

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
		const Point& getPoint() const;

	private:
		Id id = 0;
		Point point;

		/**
		 * Custom read from istreams.
		 */
		friend std::istream& operator>>(std::istream&, DataObject&);

		/**
		 * Custom print to ostreams
		 */
		friend std::ostream& operator<<(std::ostream&, const DataObject&);
};
