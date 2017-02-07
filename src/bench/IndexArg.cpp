#include "IndexArg.hpp"

#include "bench/indexes/naive/SpatialIndex.hpp"
#include "bench/indexes/parallel/SpatialIndex.hpp"
#include "bench/indexes/sequential/SpatialIndex.hpp"
#include "bench/indexes/vectorized/SpatialIndex.hpp"
#include "bench/indexes/rtree/QuadraticTree.hpp"
#include "bench/indexes/rtree/RStarTree.hpp"
#include "bench/indexes/rtree/RRStarTree.hpp"
#include "bench/indexes/rtree/GreeneTree.hpp"
#include "bench/indexes/rtree/HilbertRtree.hpp"


std::shared_ptr<SpatialIndex> IndexArg::getValue(unsigned dimension) {

	std::string algorithm = TCLAP::UnlabeledValueArg<std::string>::getValue();

	if (algorithm == "naive") {
		return std::make_shared<Naive::SpatialIndex>();
	} else if (algorithm == "parallel") {
		return std::make_shared<Parallel::SpatialIndex>();
	} else if (algorithm == "sequential") {
		return std::make_shared<Sequential::SpatialIndex>();
	} else if (algorithm == "vectorized") {
		return std::make_shared<Vectorized::SpatialIndex>();
	} else if (algorithm.substr(0, 5) == "rtree") {

		std::string type = "guttman";

		if (algorithm.size() > 5) {
			type = algorithm.substr(6, std::string::npos);
		}

		if (type == "star") {
			return createRtree<Rtree::RStarTree>(dimension);
		}

		if (type == "rstar") {
			return createRtree<Rtree::RRStarTree>(dimension);
		}

		if (type == "guttman") {
			return createRtree<Rtree::QuadraticRtree>(dimension);
		}
		
		if (type == "greene") {
			return createRtree<Rtree::GreeneRtree>(dimension);
		}

		if (type == "hilbert") {
			return createRtree<Rtree::HilbertRtree>(dimension);
		}

		throw std::invalid_argument(type + " is not a valid rtree type");
	}

	throw std::invalid_argument(algorithm + " is not a valid algorithm name");
}


std::string IndexArg::getName()
{
	return TCLAP::UnlabeledValueArg<std::string>::getValue();
};


template<template<unsigned, unsigned> class I>
std::shared_ptr<SpatialIndex> IndexArg::createRtree(unsigned dimension) {
	const unsigned C = 128;

	switch (dimension) {
		case 2:
			return std::make_shared<I<2, C>>();
		case 3:
			return std::make_shared<I<3, C>>();
		case 5:
			return std::make_shared<I<5, C>>();
		case 9:
			return std::make_shared<I<9, C>>();
		case 16:
			return std::make_shared<I<16, C>>();
		case 22:
			return std::make_shared<I<22, C>>();

		default:
			throw std::domain_error(
					"R-tree not compiled for dimension "
						+ std::to_string(dimension)
				);
	}
}
