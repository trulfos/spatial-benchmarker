#pragma once
#include <string>
#include <tclap/MultiArg.h>

/**
 * Argument returning a set of algorithms.
 *
 * The main difference between this one and a regular MultiArg is that the value
 * will default to all algorithms.
 */
class AlgorithmArg : public TCLAP::MultiArg<std::string>
{

	public:

		using MultiArg<std::string>::MultiArg;

		const container_type& getValue() const;
		const_iterator begin() const;
		const_iterator end() const;
};
