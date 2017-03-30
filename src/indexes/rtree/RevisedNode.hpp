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
 *
 * The only difference from a regular node is that the node also contains the
 * MBR of the node from the initial entries.
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
			centerEntries = this->size();
		}


		/**
		 * Calculate the original MBR.
		 */
		M originalMbr() const
		{
			assert(centerEntries > 0);
			assert(centerEntries <= this->size());

			return std::accumulate(
					this->entries, this->entries + centerEntries,
					this->entries[0].getMbr(),
					[](const M& s, const E& e) { return s + e.getMbr(); }
				);
		}

	private:
		unsigned centerEntries = 0;
};

}
