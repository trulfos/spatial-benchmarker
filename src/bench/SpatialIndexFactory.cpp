#include "SpatialIndexFactory.hpp"

#include "bench/indexes/naive/SpatialIndex.hpp"
#include "bench/indexes/parallel/SpatialIndex.hpp"
#include "bench/indexes/rtree/QuadraticTree.hpp"
#include "bench/indexes/rtree/RStarTree.hpp"
#include "bench/indexes/rtree/RRStarTree.hpp"
#include "bench/indexes/rtree/GreeneTree.hpp"
#include "bench/indexes/rtree/HilbertRtree.hpp"
#include "bench/indexes/sequential/SpatialIndex.hpp"
#include "bench/indexes/vectorized/SpatialIndex.hpp"


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

std::vector<std::string> SpatialIndexFactory::keys()
{
	return {"naive", "parallel", "rtree", "sequential", "vectorized"};
}

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
		case 4:
			return std::make_shared<I<4, C>>(dataSet);
		case 5:
			return std::make_shared<I<5, C>>(dataSet);
		case 6:
			return std::make_shared<I<6, C>>(dataSet);
		case 7:
			return std::make_shared<I<7, C>>(dataSet);
		case 8:
			return std::make_shared<I<8, C>>(dataSet);
		case 9:
			return std::make_shared<I<9, C>>(dataSet);
		case 10:
			return std::make_shared<I<10, C>>(dataSet);

		default:
			throw std::domain_error(
					"R-tree not compiled for dimension "
						+ std::to_string(dimension)
				);
	}
}
