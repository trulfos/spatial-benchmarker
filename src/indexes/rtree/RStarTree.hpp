#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "Rtree.hpp"
#include "Algorithm.hpp"
#include "Node.hpp"
#include "Entry.hpp"
#include "CoveringSet.hpp"


namespace Rtree
{

/**
 * R*-tree.
 *
 * @tparam D Dimension
 * @tparam C Node capacity
 * @tparam m Minimum number of children in each node
 * @tparam p Number of entries to reinsert (on first overflow)
 */
template<unsigned D, unsigned C, unsigned m, unsigned p>
class RStarTree : public Rtree<Node<D, C, Entry>, m>
{
	static_assert(p < C, "p must be less than the node capacity C");
	static_assert(2 * m <= C, "Min node fill level must be below capacity / 2");

	public:
		using N = Node<D, C, Entry>;
		using E = typename N::Entry;
		using M = typename E::M;


		/**
		 * Insert an entry in the tree.
		 *
		 * @param index Index to insert into
		 * @param object DataObject to insert
		 */
		void insert(const DataObject& object) override
		{
			insert(object, 0);
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

			// No nodes - set entry as root
			if (this->getHeight() == 0) {
				this->addLevel(entry);
				return;
			}

			// Single entry - add new root
			if (this->getHeight() == 1) {
				this->addLevel(
						E(this->allocateNode(), {this->getRoot(), entry})
					);
				return;
			}

			std::vector<E *> path {&this->getRoot()};

			// Find leaf node
			for (unsigned i = 0; i < height - 2 - level; i++) {
				path.back()->getMbr() += entry.getMbr();
				E& e = chooseSubtree(*path.back(), entry, height - i - 1);
				path.push_back(&e);
			}

			// Split nodes bottom-up as long as necessary
			E e = entry;
			auto top = path.rbegin();

			while (top != path.rend() && (*top)->getNode()->isFull()) {

				// Reinsert entries
				if (!split) {
					for (E& i : extractEntries(**top, e)) {
						insert(i, top - path.rbegin(), true);
					}

					// Adjust bounding rectangles upwards in the tree
					for (auto n = top + 1; n != path.rend(); n++) {
						(*n)->recalculateMbr();
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
				this->addLevel(
						E(this->allocateNode(), {**path.begin(), e})
					);
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
						return (a.getMbr().center() - parent.getMbr().center()).squared()
							< (b.getMbr().center() - parent.getMbr().center()).squared();
					}
				);

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
			CoveringSet<E> covering (parent.begin(), parent.end(), newEntry);

			if (!covering.empty()) {
				return covering.minBy(&M::volume);
			}

			if (elevation != 2) {
				return leastVolumeEnlargement(parent, newEntry);
			}

			return leastOverlapEnlargement(parent, newEntry);
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
						return entry.getMbr().delta(&M::volume, newEntry.getMbr());
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
			// Sort by volume
			std::sort(
					parent.begin(), parent.end(),
					[](const E& a, const E&b) {
						return a.getMbr().volume() < b.getMbr().volume();
					}
				);

			// Find entry with no enlargement (if any)
			auto noEnlargementEntry = std::find_if(
					parent.begin(), parent.end(),
					[&](const E& e) {
						return overlap(parent, e.getMbr() + newEntry.getMbr())
								== overlap(parent, e.getMbr());
					}
				);

			if (noEnlargementEntry != parent.end()) {
				return *noEnlargementEntry;
			}

			return *argmin(
					parent.begin(), parent.end(),
					[&](const E& entry) {
						// Calculate overlap enlargement
						double o = overlap(parent, entry.getMbr() + newEntry.getMbr())
							- overlap(parent, entry.getMbr());

						// Resolve ties using volume enlargement
						return std::make_tuple(
								o,
								entry.getMbr().delta(&M::volume, newEntry.getMbr())
							);
					}
				);
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
		double overlap(E& parent, M mbr)
		{
			return std::accumulate(
					parent.begin(),
					parent.end(),
					0.0f,
					[&](const double& sum, const E& entry) {
						return mbr.intersects(entry.getMbr()) ?
							sum + mbr.intersection(entry.getMbr()).volume() : sum;
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

			// This is assumed further down
			assert(entries.size() > 1);


			// Choose split dimension and index
			unsigned bestDimension = E::dimension;
			unsigned bestSplit = entries.size();
			double minPerimeter = std::numeric_limits<double>::infinity();

			for (unsigned d = 0; d < E::dimension; ++d) {

				// Sort along current dimension
				std::sort(
						entries.begin(),
						entries.end(),
						[&](const E& a, const E& b) {

							auto diff = a.getMbr().getBottom()[d]
								- b.getMbr().getBottom()[d];

							if (diff == 0.0f) {
								return diff < 0.0f;
							}

							return a.getMbr().getTop()[d] < b.getMbr().getTop()[d];
						}
					);

				// Determine best split and best perimeter for this dimension
				unsigned localBestSplit = entries.size();
				double minOverlap = std::numeric_limits<double>::infinity();

				for (unsigned s = m; s < entries.size() - m; ++s) {

					// Generate MBRs for the two groups
					auto mbrA = entries.front().getMbr();
					auto mbrB = entries.back().getMbr();

					for (unsigned i = 1; i < s; ++i) {
						mbrA += entries[i].getMbr();
					}

					for (unsigned i = s; i < entries.size() - 1; ++i) {
						mbrB += entries[i].getMbr();
					}

					// Calculate perimeter (for dimension selection)
					double perimeter = mbrA.perimeter() + mbrB.perimeter();
					if (perimeter < minPerimeter) {
						bestDimension = d;
						minPerimeter = perimeter;
					}

					// Calcualte overlap (for split point selection)
					double overlap = mbrA.intersects(mbrB) ?
							mbrA.intersection(mbrB).volume() : 0.0f;

					if (overlap < minOverlap) { //TODO: Fallback to perimeter or volume?
						minOverlap = overlap;
						localBestSplit = s;
					}
				}

				assert(localBestSplit < entries.size());

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

						auto diff = a.getMbr().getBottom()[bestDimension]
							- b.getMbr().getBottom()[bestDimension];

						if (diff == 0.0f) {
							return diff < 0.0f;
						}

						return a.getMbr().getTop()[bestDimension]
							< b.getMbr().getTop()[bestDimension];
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
