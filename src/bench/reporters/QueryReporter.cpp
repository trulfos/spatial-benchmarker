#include "QueryReporter.hpp"

QueryReporter::QueryReporter(const std::string& queryPath) : path(queryPath)
{
}

LazyQuerySet QueryReporter::getQuerySet() const
{
	return LazyQuerySet(path);
}
