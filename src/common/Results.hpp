#pragma once

#include <vector>
#include "DataObject.hpp"

/**
 * Represents a set of results.
 *
 * Note that the results only contains the object ids and not the objects
 * them selves. This simplifies the index because it does not have to
 * reconstruct the object itself.
 *
 * Currently just a shortcut for std::vector.
 */
typedef std::vector<DataObject::Id> Results;

/**
 * Specialized method for extraction from stream.
 */
std::istream& operator>>(std::istream& stream, Results& resultSet);
