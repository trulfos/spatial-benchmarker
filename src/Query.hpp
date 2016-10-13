#pragma once

#include "Point.hpp"

/**
 * Represents a query.
 *
 * The query may be either a range query or a k-nn query.
 */
class Query
{
	public:
		enum Type{RANGE, KNN};
		Type getType() const;

	protected:
		Query(Type type);

	private:
		Type type;

};
