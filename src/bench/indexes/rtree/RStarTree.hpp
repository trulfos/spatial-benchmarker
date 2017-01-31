#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "Rtree.hpp"
#include "common/Algorithm.hpp"
#include "Node.hpp"
#include "Entry.hpp"


namespace Rtree
{

/**
 * R*-tree.
 *
 * @tparam D Dimension
 * @tparam C Node capacity
 */
template<unsigned D, unsigned C>
class RStarTree : public Rtree<Node<D, C, Entry>>
{
	public:
		using N = Node<D, C, Entry>;
		using E = typename N::Entry;
		using M = typename E::M;

		RStarTree(LazyDataSet& dataSet)
		{
			this->load(dataSet);
		};

		/**
		 * Insert an entry in the tree.
		 *
		 * @param index Index to insert into
		 * @param object DataObject to insert
		 */
		void insert(const E& entry) override
		{
			insert(entry, 0);
		};


	private:
		/**
		 * Insert an entry in the tree.
		 *
		 * @param index Index to insert into
		 * @param object DataObject to insert
		 * @param split Last split level (used in recursive calls)
		 */
		void insert(const E& entry, unsigned level, bool split = false)
		{
			const unsigned& height = this->getHeight();

			E rootEntry (this->getRoot(), M());
			std::vector<E *> path {&rootEntry};

			// Find leaf node
			for (unsigned i = 0; i < height - 1 - level; i++) {
				E& e = chooseSubtree(*path.back(), entry, height - i);
				e.mbr += entry.mbr;
				path.push_back(&e);
			}

			// Split nodes bottom-up as long as necessary
			E e = entry;
			auto top = path.rbegin();

			while (top != path.rend() && (*top)->node->isFull()) {

				// Reinsert entries
				if (!split) {
					for (E& i : extractEntries(**top, e)) {
						insert(i, top - path.rbegin(), true);
					}
					return;
				}

				// Split node
				e = E(this->allocateNode(), {e});
				redistribute(**top, e);

				++top;
			}

			// Split root?
			if (top == path.rend()) {
				E newRoot (this->allocateNode(), {**path.begin(), e});
				this->addLevel(newRoot.node);
			} else {
				(*top)->add(e);
			}
		};


		/**
		 * Extracts the most distant entries from the given entry.
		 *
		 * @param parent Parent from who's children entries should be extracted
		 * @param newEntry Extra entry to include
		 *
		 * @return Extracted entries ordered bycenter distance to parent
		 */
		std::vector<E> extractEntries(E& parent, const E& newEntry)
		{
			unsigned p = E::Node::capacity - E::Node::capacity / 2;

			// Collect all entries
			std::vector<E> entries (
					parent.begin(),
					parent.end()
				);

			entries.push_back(newEntry);


			// Sort by center distance to parent
			std::sort(
					entries.begin(),
					entries.end(),
					[&](const E& a, const E& b) {
						return (a.mbr.center() - parent.mbr.center()).squared()
							< (b.mbr.center() - parent.mbr.center()).squared();
					}
				);

			assert(p < E::Node::capacity);
			assert(entries.size() > 0);

			auto middle = entries.end() - p;

			// Add back remaining entries to node
			parent.assign(
					entries.begin(),
					middle
				);

			// Return extracted entries
			return std::vector<E>(
					middle,
					entries.end()
				);
		};


		/**
		 * Find a suitable subtree for the entry.
		 *
		 * @param entry Entry to find location for
		 * @param node Node in which the subtree should be
		 * @param elevation Node elevation (from bottom of tree)
		 */
		E& chooseSubtree(E& parent, const E& newEntry, unsigned elevation)
		{
			if (elevation == 2) {
				return leastOverlapEnlargement(parent, newEntry);
			}

			return leastVolumeEnlargement(parent, newEntry);
		};


		/**
		 * Find the child requiring the least MBR enlargement to include the
		 * given MBR.
		 *
		 * @param mbr MBR to include
		 * @return Entry requiring the least enlargement to include mbr
		 */
		E& leastVolumeEnlargement(E& parent, const E& newEntry)
		{
			return *argmin(
					parent.begin(),
					parent.end(),
					[&](const E& entry) {
						return entry.mbr.enlargement(newEntry.mbr);
					}
				);
		}

		/**
		 * Find the child having the least overlap enlargement with the given
		 * MBR.
		 *
		 * @param mbr MBR to include
		 * @return Entry with the least overlap
		 */
		E& leastOverlapEnlargement(E& parent, const E& newEntry)
		{
			//TODO: Drag the algorithm out into a template?
			E * best = parent.node->entries;
			float minimum = std::numeric_limits<float>::infinity();

			for (E& entry : *(parent.node)) {

				// Calculate overlap enlargement
				float o = overlap(parent, entry.mbr + newEntry.mbr)
					- overlap(parent, entry.mbr);

				if (o < minimum) {
					minimum = o,
					best = &entry;
				}

				// Resolve ties using volume enlargement
				if (fabs(o - minimum) < 0.000001f) {
					if (
						best->mbr.enlargement(newEntry.mbr)
							> entry.mbr.enlargement(newEntry.mbr)
					) {
						best = &entry;
					}
				}
			}

			return *best;
		};


		/**
		 * Calculate the overlap of an MBR with all the children of a entry's
		 * node.
		 *
		 * @param parent Parent entry who's children will be used
		 * @param mbr MBR to calculate overlap for
		 *
		 * @return Overlap of the MBR with the children
		 */
		float overlap(E& parent, M mbr)
		{
			return std::accumulate(
					parent.begin(),
					parent.end(),
					0.0f,
					[&](const float& sum, const E& entry) {
						return mbr.intersects(entry.mbr) ?
							sum + mbr.intersection(entry.mbr).volume() : sum;
					}
				);
		};


		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The first entry (with children)
		 * @param b The second entry (with children)
		 */
		void redistribute(E& a, E& b)
		{
			// Contruct buffer with all entries
			std::vector<E> entries (a.begin(), a.end());

			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);

			//TODO: This value should be changeable
			const unsigned m = E::Node::capacity / 2;

			// This is assumed further down
			assert(entries.size() > 1);


			// Choose split dimension and index
			unsigned bestDimension = E::dimension;
			unsigned bestSplit = entries.size();
			float minPerimeter = std::numeric_limits<float>::infinity();

			for (unsigned d = 0; d < E::dimension; ++d) {

				// Sort along current dimension
				std::sort(
						entries.begin(),
						entries.end(),
						[&](const E& a, const E& b) {

							auto diff = a.mbr.getBottom()[d]
								- b.mbr.getBottom()[d];

							if (diff == 0.0f) {
								return diff < 0.0f;
							}

							return a.mbr.getTop()[d] < b.mbr.getTop()[d];
						}
					);

				// Determine best split and best perimeter for this dimension
				unsigned localBestSplit = entries.size();
				float minOverlap = std::numeric_limits<float>::infinity();

				for (unsigned s = m; s < entries.size() - m; ++s) {

					// Generate MBRs for the two groups
					auto mbrA = entries.front().mbr;
					auto mbrB = entries.back().mbr;

					for (unsigned i = 1; i < s; ++i) {
						mbrA += entries[i].mbr;
					}

					for (unsigned i = s; i < entries.size() - 1; ++i) {
						mbrB += entries[i].mbr;
					}

					// Calculate perimeter (for dimension selection)
					float perimeter = mbrA.perimeter() + mbrB.perimeter();
					if (perimeter < minPerimeter) {
						bestDimension = d;
						minPerimeter = perimeter;
					}

					// Calcualte overlap (for split point selection)
					float overlap = mbrA.intersects(mbrB) ?
							mbrA.intersection(mbrB).volume() : 0.0f;

					if (overlap < minOverlap) { //TODO: Fallback to perimeter or volume?
						minOverlap = overlap;
						localBestSplit = s;
					}
				}

				if (bestDimension == d) {
					bestSplit = localBestSplit;
				}
			}

			// A dimension and split should have been chosen
			assert(bestDimension < E::dimension);
			assert(bestSplit >= m && bestSplit < entries.size() - m);

			// Distribute entries
			auto middle = entries.begin() + bestSplit;

			std::nth_element(
					entries.begin(),
					middle,
					entries.end(),
					[&](const E& a, const E& b) {

						auto diff = a.mbr.getBottom()[bestDimension]
							- b.mbr.getBottom()[bestDimension];

						if (diff == 0.0f) {
							return diff < 0.0f;
						}

						return a.mbr.getTop()[bestDimension]
							< b.mbr.getTop()[bestDimension];
					}
				);

			a.assign(
					entries.begin(),
					middle
			   );

			b.assign(
					middle,
					entries.end()
				);
		};
};

}