#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <set>
#include "Rtree.hpp"
#include "common/Algorithm.hpp"
#include "Entry.hpp"
#include "RevisedNode.hpp"
#include "WeightingFunction.hpp"
#include "GoalFunction.hpp"
#include "Split.hpp"
#include "SplitSet.hpp"
#include "CoveringSet.hpp"
#include "ReferenceView.hpp"
#include "CheckComp.hpp"

namespace Rtree
{

/**
 * Revised R*-tree.
 *
 * @tparam D Dimension
 * @tparam C Node capacity
 * @tparam m Minimum number of children in each node
 */
template<unsigned D, unsigned C, unsigned m>
class RRStarTree : public Rtree<RevisedNode<D, C, Entry>>
{
	public:

		using N = RevisedNode<D, C, Entry>;
		using E = Entry<D, N>;
		using M = typename E::M;



		/**
		 * Insert an entry in the tree.
		 *
		 * @param object DataObject to insert
		 */
		virtual void insert(const DataObject& object) override
		{
			E entry (object);
			E rootEntry (this->getRoot(), M());
			std::vector<E *> path {&rootEntry};

			//TODO: This is a hack
			if (rootEntry.node->nEntries > 0) {
				rootEntry.mbr = rootEntry.node->originalMbr();
			}

			// Find leaf node
			for (unsigned i = 0; i < this->getHeight() - 1; i++) {
				E& e = chooseSubtree(*path.back(), entry);
				e.mbr += entry.mbr;
				path.push_back(&e);
			}

			// Split nodes bottom-up as long as necessary
			auto top = path.rbegin();

			while (top != path.rend() && (*(top))->node->isFull()) {
				entry = E(this->allocateNode(), {entry});
				redistribute(**top, entry, top == path.rbegin());
				top++;
			}

			// Split root?
			if (top == path.rend()) {
				E newRoot (this->allocateNode(), {**path.begin(), entry});
				this->addLevel(newRoot.node);
			} else {
				(*top)->add(entry);
			}
		};


		/**
		 * Provide a couple of extra statistics.
		 */
		StatsCollector collectStatistics() const override
		{
			auto stats = Rtree<N>::collectStatistics();
			stats["perimeter_splits"] = perimeterSplits;
			return stats;
		}


	private:

		unsigned long long perimeterSplits = 0;

		/**
		 * Find a suitable subtree for the entry.
		 *
		 * @param entry Entry to find location for
		 * @param node Node in which the subtree should be
		 */
		E& chooseSubtree(E& parent, const E& newEntry)
		{
			// Construct set of covering entries
			CoveringSet<E> covering (parent.begin(), parent.end(), newEntry);

			if (!covering.empty()) {
				if (!covering.allHasVolume()) {
					return covering.getMinPerimeter();
				}

				return covering.getMinVolume();
			}

			// Create a sorted view of the children (by delta perimeter)
			ReferenceView<E> children (parent.begin(), parent.end());

			children.sort([&](const E& a, const E& b) {
					return (a.mbr + newEntry.mbr).perimeter() - a.mbr.perimeter()
						< (b.mbr + newEntry.mbr).perimeter() - b.mbr.perimeter();
				});


			// Optimize by returning the first one if its overlap perimeter
			// enlargement is 0 for the new entry
			if (
					std::all_of(
							children.begin() + 1, children.end(),
							[&](const E& entry) {
								return children[0].mbr.overlapEnlargement(
										entry.mbr, newEntry.mbr, &M::perimeter
									) == 0.0;
							}
						)
			) {
				return children[0];
			}

			// Determine threshold (optimization)
			auto p = children.begin() + 1;

			for (auto i = children.begin() + 1; i != children.end(); ++i) {
				double deltaOvlp = children[0].mbr.overlapEnlargement(
						i->mbr,
						newEntry.mbr,
						&M::perimeter
					);

				if (deltaOvlp > 0.0) {
					p = i + 1;
				}
			}

			return *CheckComp<typename decltype(children)::iterator, E>(
							children.begin(),
							p,
							newEntry
						)();
		};


		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The old entry (with children)
		 * @param b The new entry (with the new child)
		 */
		void redistribute(E& a, E& b, bool isLeaf = false) //TODO: isLeaf
		{
			// Functions for evaluating splits
			GoalFunction<E> wg (a.mbr + b.mbr);
			WeightingFunction<E, m> wf (a);

			// Construct set of possible splits
			SplitSet<E, m> splits (
					a.begin(), b.begin(),
					a.end(), b.end()
				);


			// Restrict to single dimension for leafs
			if (isLeaf) {
				const Split<E>& split = *argmin(
							splits.begin(), splits.end(),
							[](const Split<E>& split) {
								return split.perimeter();
							}
						);

				splits.restrictTo(split.getDimension());
			}

			// Can we use volume?
			bool useVolume = std::all_of(
					splits.begin(), splits.end(),
					[](const Split<E>& split) {
						return split.hasVolume();
					}
				);

			if (!useVolume) {
				perimeterSplits++;
			}

			// Determine best split
			Split<E> split = *argmin(
					splits.begin(), splits.end(),
					[&](const Split<E>& split) {

						// Evaluate goal and weight functions
						wf.setDimension(split.getDimension());
						double g = wg(split, useVolume);
						double f = wf(split.getSplitPoint());

						return g < 0.0 ? g * f : g / f;
					}
				);

			// Distribute entries
			auto partitions = split.getEntries();
			a.assign(partitions[0].begin(), partitions[0].end());
			b.assign(partitions[1].begin(), partitions[1].end());

			// Update center positions
			a.node->captureMbr();
			b.node->captureMbr();
		}
};

}
