#pragma once
#include <vector>
#include <algorithm>
#include "BasicRtree.hpp"
#include "Algorithm.hpp"
#include "Entry.hpp"
#include "RevisedNode.hpp"
#include "WeightingFunction.hpp"
#include "GoalFunction.hpp"
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
class RRStarTree : public BasicRtree<RevisedNode<D, C, Entry>, m>
{
	public:

		using N = RevisedNode<D, C, Entry>;
		using E = Entry<D, N>;
		using M = typename E::M;

		/**
		 * Provide a couple of extra statistics.
		 */
		StatsCollector collectStatistics() const override;


	private:

		unsigned long long perimeterSplits = 0;

		/**
		 * Find a suitable subtree for the entry.
		 *
		 * @param entry Entry to find location for
		 * @param node Node in which the subtree should be
		 */
		E& chooseSubtree(E& parent, const E& newEntry) override;


		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The old entry (with children)
		 * @param b The new entry (with the new child)
		 * @param level Level of entries
		 */
		void redistribute(E& a, E& b, unsigned level) override;
};



/*
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
*/

template<unsigned D, unsigned C, unsigned m>
StatsCollector RRStarTree<D, C, m>::collectStatistics() const
{
	auto stats = Rtree<N, m>::collectStatistics();
	stats["perimeter_splits"] = perimeterSplits;
	return stats;
}


template<unsigned D, unsigned C, unsigned m>
typename RRStarTree<D, C, m>::E& RRStarTree<D, C, m>::chooseSubtree(
		E& parent,
		const E& newEntry
	)
{
	// Construct set of covering entries
	CoveringSet<E> covering (parent.begin(), parent.end(), newEntry);

	if (!covering.empty()) {
		return covering.minBy(
				covering.allHasVolume() ? &M::volume : &M::perimeter
			);
	}

	// Create a sorted view of the children (by delta perimeter)
	ReferenceView<E> children (parent.begin(), parent.end());

	children.sort([&](const E& a, const E& b) {
			return a.mbr.delta(&M::perimeter, newEntry.mbr)
				< b.mbr.delta(&M::perimeter, newEntry.mbr);
		});


	// Return the first one if its overlap perimeter enlargement is 0
	if (
			std::all_of(
					children.begin() + 1, children.end(),
					[&](const E& entry) {
						return children[0].mbr.deltaOverlap(
								entry.mbr, newEntry.mbr, &M::perimeter
							) == 0.0;
					}
				)
	) {
		return children[0];
	}

	// Construct and run CheckComp
	using EIt = typename decltype(children)::iterator;
	CheckComp<EIt> checkComp(children.begin(), children.end(), newEntry);

	auto result = checkComp(children.begin());

	if (result != children.end()) {
		return *result;
	}

	return checkComp.minOverlap();
};


template<unsigned D, unsigned C, unsigned m>
void RRStarTree<D, C, m>::redistribute(E& a, E& b, unsigned level)
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
	if (level == this->getHeight() - 1) {
		const Split<E> split = *argmin(
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

}
