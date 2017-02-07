#pragma once
#include <tclap/UnlabeledValueArg.h>
#include <memory>
#include "SpatialIndex.hpp"
#include "LazyDataSet.hpp"

class IndexArg : public TCLAP::UnlabeledValueArg<std::string>
{
	public:

		using TCLAP::UnlabeledValueArg<std::string>::UnlabeledValueArg;

		/**
		 * Create a new index of this arguments type.
		 *
		 * @return Shared pointer to index
		 */
		std::shared_ptr<SpatialIndex> getValue(unsigned dimension);


		/**
		 * Get the name of the algorithm selected by this argument.
		 *
		 * @return Name of algorithm
		 */
		std::string getName();


	private:

		/**
		 * Creates a new R-tree index.
		 *
		 * @tparam S Insertion strategy
		 * @param dimension Dimension of the new R-tree
		 */
		template<template<unsigned, unsigned> class I>
		static std::shared_ptr<SpatialIndex> createRtree(unsigned dimension);
};
