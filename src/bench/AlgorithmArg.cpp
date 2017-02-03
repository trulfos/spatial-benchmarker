#include "AlgorithmArg.hpp"
#include "SpatialIndexFactory.hpp"

const AlgorithmArg::container_type& AlgorithmArg::getValue() const
{
	if (_values.empty()) {
		return SpatialIndexFactory::keys;
	}

	return _values;
}

AlgorithmArg::const_iterator AlgorithmArg::begin() const
{
	return getValue().cbegin();
}

AlgorithmArg::const_iterator AlgorithmArg::end() const
{
	return getValue().cend();
}
