#pragma once
#include "Results.hpp"
#include <vector>
#include <istream>

typedef std::vector<Results> ResultSet;


std::istream& operator>>(std::istream& stream, ResultSet& resultSet);
