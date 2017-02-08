#include "hilbert.hpp"
#include "bench/indexes/rtree/HilbertRtree.hpp"

template<unsigned D, unsigned C>
using I = Rtree::HilbertRtree<D, C>;

SpatialIndex * create()
{

	const unsigned C = 128;

	return new Rtree::HilbertRtree<3, C>();
	/*
	switch (dimension) {
		case 2:
			return new Rtree::HilbertRtree<2, C>();
		case 3:
			return new Rtree::HilbertRtree<3, C>();
		case 5:
			return new Rtree::HilbertRtree<5, C>();
		case 9:
			return new Rtree::HilbertRtree<9, C>();
		case 16:
			return new Rtree::HilbertRtree<16, C>();
		case 22:
			return new Rtree::HilbertRtree<22, C>();


		default:
			throw std::domain_error(
					"Hilbert R-tree not compiled for dimension "
						+ std::to_string(dimension)
				);
	}
	*/
}

void destroy(SpatialIndex * index)
{
	delete index;
}
