#include "Query.hpp"

namespace Spatial
{

Query::Query(Type type)
	: type(type)
{
};

Query::Type Query::getType() const
{
	return type;
}

}
