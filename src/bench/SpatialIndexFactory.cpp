#include "SpatialIndexFactory.hpp"

#include "bench/indexes/naive/SpatialIndex.hpp"
#include "bench/indexes/parallel/SpatialIndex.hpp"
#include "bench/indexes/sequential/SpatialIndex.hpp"
#include "bench/indexes/vectorized/SpatialIndex.hpp"
#include "bench/indexes/rtree/QuadraticTree.hpp"
#include "bench/indexes/rtree/RStarTree.hpp"
#include "bench/indexes/rtree/RRStarTree.hpp"
#include "bench/indexes/rtree/GreeneTree.hpp"
#include "bench/indexes/rtree/HilbertRtree.hpp"


std::shared_ptr<SpatialIndex> SpatialIndexFactory::create(
		const std::string& algorithm,
		LazyDataSet& dataSet
) {
	if (algorithm == "naive") {
		return std::make_shared<Naive::SpatialIndex>(dataSet);
	} else if (algorithm == "parallel") {
		return std::make_shared<Parallel::SpatialIndex>(dataSet);
	} else if (algorithm == "sequential") {
		return std::make_shared<Sequential::SpatialIndex>(dataSet);
	} else if (algorithm == "vectorized") {
		return std::make_shared<Vectorized::SpatialIndex>(dataSet);
	} else if (algorithm.substr(0, 5) == "rtree") {
		unsigned d = dataSet.getDimension();

		std::string type = "guttman";

		if (algorithm.size() > 5) {
			type = algorithm.substr(6, std::string::npos);
		}

		if (type == "star") {
			return createRtree<Rtree::RStarTree>(d, dataSet);
		}

		if (type == "rstar") {
			return createRtree<Rtree::RRStarTree>(d, dataSet);
		}

		if (type == "guttman") {
			return createRtree<Rtree::QuadraticRtree>(d, dataSet);
		}
		
		if (type == "greene") {
			return createRtree<Rtree::GreeneRtree>(d, dataSet);
		}

		if (type == "hilbert") {
			return createRtree<Rtree::HilbertRtree>(d, dataSet);
		}

		throw std::invalid_argument(type + " is not a valid rtree type");
	}

	throw std::invalid_argument(algorithm + " is not a valid algorithm name");
}

const std::vector<std::string> SpatialIndexFactory::keys = {
		"naive", "parallel", "sequential", "vectorized", "rtree", "rtree-star",
		"rtree-rstar", "rtree-hilbert"
	};

template<template<unsigned, unsigned> class I>
std::shared_ptr<SpatialIndex> SpatialIndexFactory::createRtree(
		unsigned dimension,
		LazyDataSet& dataSet
) {
	const unsigned C = 128;

	switch (dimension) {
		case 2:
			return std::make_shared<I<2, C>>(dataSet);
		case 3:
			return std::make_shared<I<3, C>>(dataSet);
		case 5:
			return std::make_shared<I<5, C>>(dataSet);
		case 9:
			return std::make_shared<I<9, C>>(dataSet);
		case 16:
			return std::make_shared<I<16, C>>(dataSet);
		case 22:
			return std::make_shared<I<22, C>>(dataSet);

		default:
			throw std::domain_error(
					"R-tree not compiled for dimension "
						+ std::to_string(dimension)
				);
	}
}
