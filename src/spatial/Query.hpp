#pragma once

#include "Point.hpp"
#include <string>

/**
 * Represents a query.
 *
 * The query may be either a range query or a k-nn query.
 */
class Query
{
	public:
		enum Type{RANGE, KNN};

		Query() = default;
		virtual ~Query() = default;

		Type getType() const;
		virtual std::string getName() const = 0;

	protected:
		Query(Type type);

	private:
		Type type;

};
