#include "Query.hpp"

Query::Query(Type type)
	: type(type)
{
};

Query::Type Query::getType() const
{
	return type;
}
