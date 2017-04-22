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
class RStarTree : public Rtree<Node<D, C>, m>
{
	static_assert(p < C, "p must be less than the node capacity C");
	static_assert(2 * m <= C, "Min node fill level must be below capacity / 2");

	using N = Node<D, C>;
	using M = Mbr<D>;

	using Base = Rtree<N, m>;
	using NIt = typename N::iterator;

	public:
		// These depend on template parameters
		using Base::getHeight;
		using Base::addLevel;
		using Base::getRoot;


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
		 * @param entry Entry to insert
		 * @param level Destination level for the entry
		 * @param isReinsert True to avoid reinsertion during reinsertion
		 */
		void insert(Entry<N> entry, unsigned level, bool isReinsert = false)
		{
			const unsigned& height = getHeight();

			// No nodes - set entry as root
			if (height - level == 0) {
				addLevel(entry);
				return;
			}

			// Single entry - add new root
			if (height - level == 1) {
				addLevel(
						Entry<N>(new N({getRoot(), entry}))
					);
				return;
			}

			getRoot().include(entry);

			if (height - level > 2) {
				std::vector<NIt> path {
						chooseSubtree(getRoot().getNode(), entry, height - 1)
					};
				path.back()->include(entry);

				// Find leaf node
				for (unsigned i = 1; i < height - 2 - level; i++) {
					NIt e = chooseSubtree(
							path.back()->getNode(),
							entry,
							height - i - 1
						);

					e->include(entry);
					path.push_back(e);
				}

				// Split nodes bottom-up as long as necessary
				auto top = path.rbegin();

				while (top != path.rend() && (*top)->getNode().isFull()) {
					// Reinsert entries
					if (!isReinsert) {
						// Extract entries to reinsert
						auto extracted = extractEntries(*top, entry);

						// Adjust bounding rectangles upwards in the tree
						for (auto n = top; n != path.rend(); n++) {
							(*n)->recalculate();
						}
						getRoot().recalculate();

						// Reinsert
						for (Entry<N>& i : extracted) {
							insert(i, top - path.rbegin(), true);
						}

						return;
					}

					// Split node
					entry = split(**top, entry);
					++top;
				}

				// Can we do final insert?
				if (top != path.rend()) {
					(*top)->getNode().add(entry);
					return;
				}
			}

			// We must insert into the root
			N& node = getRoot().getNode();

			// Split root?
			if (node.isFull()) {
				entry = split(getRoot(), entry);
				addLevel(
						Entry<N>(new N({getRoot(), entry}))
					);
				return;
			}

			// Regular insert
			node.add(entry);
		};


		/**
		 * Split the node of an entry and include an extra child.
		 *
		 * This will create a new node with the related entry and split the
		 * entries of parent between parent and the new node.
		 *
		 * @param parent Parent entry with node to split
		 * @param entry Extra child to include in split
		 * @return New entry with new node
		 */
		template<class E>
		Entry<N> split(E& parent, Entry<N> entry)
		{
			// Create new node
			Entry<N> newEntry (new N {entry});

			// Distribute children
			redistribute(
					newEntry.getNode(),
					parent.getNode()
				);

			// Recalculate entries
			newEntry.recalculate();
			parent.recalculate();

			return newEntry;
		}


		/**
		 * Extracts the most distant entries from the given entry.
		 *
		 * @param parent Parent from which child entries should be extracted
		 * @param newEntry Extra entry to include
		 *
		 * @return Extracted entries ordered bycenter distance to parent
		 */
		std::vector<Entry<N>> extractEntries(
				NIt parent,
				const Entry<N>& newEntry
			)
		{
			// Collect all entries
			std::vector<Entry<N>> entries (
					parent->getNode().begin(),
					parent->getNode().end()
				);

			entries.push_back(newEntry);


			// Sort by center distance to parent
			auto parentCenter = parent->getMbr().center();
			std::sort(
					entries.begin(),
					entries.end(),
					[&](const Entry<N>& a, const Entry<N>& b) {
						return (a.getMbr().center() - parentCenter).squared()
							< (b.getMbr().center() - parentCenter).squared();
					}
				);

			assert(entries.size() > 0);

			auto middle = entries.end() - p;

			// Add back remaining entries to node
			parent->getNode().assign(
					entries.begin(),
					middle
				);

			// Return extracted entries
			return std::vector<Entry<N>>(
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
		NIt chooseSubtree(
				N& parent,
				const Entry<N>& newEntry,
				unsigned elevation
			)
		{
			CoveringSet<NIt> covering (parent.begin(), parent.end(), newEntry);

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
		NIt leastVolumeEnlargement(N& parent, const Entry<N>& newEntry)
		{
			return argmin(
					parent.begin(),
					parent.end(),
					[&](const Entry<N>& entry) {
						return entry.getMbr().delta(
								&M::volume,
								newEntry.getMbr()
							);
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
		NIt leastOverlapEnlargement(N& parent, const Entry<N>& newEntry)
		{
			// Sort by volume
			std::sort(
					parent.begin(), parent.end(),
					[](const Entry<N>& a, const Entry<N>&b) {
						return a.getMbr().volume() < b.getMbr().volume();
					}
				);

			// Find entry with no enlargement (if any)
			auto noEnlargementEntry = std::find_if(
					parent.begin(), parent.end(),
					[&](typename N::reference e) {
						return overlap(parent, e.getMbr() + newEntry.getMbr())
								== overlap(parent, e.getMbr());
					}
				);

			if (noEnlargementEntry != parent.end()) {
				return noEnlargementEntry;
			}

			return argmin(
					parent.begin(), parent.end(),
					[&](typename N::reference entry) {
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
		double overlap(N& parent, M mbr)
		{
			return std::accumulate(
					parent.begin(),
					parent.end(),
					0.0f,
					[&](const double& sum, typename N::reference entry) {
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
		void redistribute(N& a, N& b)
		{
			// Contruct buffer with all entries
			std::vector<Entry<N>> entries (a.begin(), a.end());

			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);

			// This is assumed further down
			assert(entries.size() > 1);


			// Choose split dimension and index
			unsigned bestDimension = D;
			unsigned bestSplit = entries.size();
			double minPerimeter = std::numeric_limits<double>::infinity();

			for (unsigned d = 0; d < D; ++d) {

				// Sort along current dimension
				std::sort(
						entries.begin(),
						entries.end(),
						[&](const Entry<N>& a, const Entry<N>& b) {

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
			assert(bestDimension < D);
			assert(bestSplit >= m && bestSplit < entries.size() - m);

			// Distribute entries
			auto middle = entries.begin() + bestSplit;

			std::nth_element(
					entries.begin(),
					middle,
					entries.end(),
					[&](const Entry<N>& a, const Entry<N>& b) {

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
