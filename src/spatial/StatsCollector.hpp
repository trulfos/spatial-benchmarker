#pragma once
#include <string>
#include <map>

namespace Spatial
{

/**
 * Responsible for storing statistics collected during a search.
 */
using StatsCollector = std::map<std::string, unsigned>;

}
