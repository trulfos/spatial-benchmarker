#pragma once

#include "common/DataObject.hpp"
#include <istream>
#include <vector>

/**
 * This represents a data set, consisting of data objects.
 *
 * Currently just an alias for vector with the right type parameter.
 */
typedef std::vector<DataObject> DataSet;


std::istream& operator>>(std::istream& stream, DataSet& dataSet);
std::ostream& operator<<(std::ostream& stream, const DataSet& dataSet);
