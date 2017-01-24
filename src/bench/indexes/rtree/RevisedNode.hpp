#pragma once
#include "Node.hpp"
#include "common/Coordinate.hpp"
#include <array>
#include <cassert>
#include <numeric>

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
		using M = typename E::M;


		/**
		 * Set the number of entries making up the original MBR to the current
		 * number of entries in the node.
		 */
		void captureMbr()
		{
			centerEntries = this->nEntries;
		}


		/**
		 * Calculate the original MBR.
		 */
		M originalMbr()
		{
			assert(centerEntries > 0);

			return std::accumulate(
					this->entries, this->entries + centerEntries,
					this->entries[0].mbr,
					[](const M& s, const E& e) { return s + e.mbr; }
				);
		}

	private:
		unsigned centerEntries = 1;
};

}
