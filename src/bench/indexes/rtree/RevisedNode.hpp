#pragma once
#include "Node.hpp"

namespace Rtree
{

/**
 * Specialized node for the revised R*-tree.
 */
template <unsigned D, unsigned C, template<unsigned, class> class Entry>
class RevisedNode : public BaseNode<D, C, RevisedNode<D, C, Entry>, Entry>
{
	public:
		using E = Entry<D, RevisedNode>;
		float oldCenter[D];

};

}
