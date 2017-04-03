#pragma once
#include "QueryInputIterator.hpp"
#include "FileSet.hpp"

namespace Bench
{

using LazyQuerySet = FileSet<QueryInputIterator>;

}
