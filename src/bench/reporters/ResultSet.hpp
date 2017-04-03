#pragma once
#include "spatial/Results.hpp"
#include <vector>
#include <istream>

using namespace Spatial;

namespace Bench
{

typedef std::vector<Results> ResultSet;


std::istream& operator>>(std::istream& stream, ResultSet& resultSet);
std::ostream& operator<<(std::ostream& stream, const ResultSet& resultSet);

}
