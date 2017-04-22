#pragma once
#include <vector>
#include <algorithm>
#include "Range.hpp"
#include "BasicRtree.hpp"
#include "Algorithm.hpp"
#include "CapturingEntryPlugin.hpp"
#include "WeightingFunction.hpp"
#include "GoalFunction.hpp"
#include "SplitSet.hpp"
#include "CoveringSet.hpp"
#include "ReferenceView.hpp"
#include "CheckComp.hpp"
#include "Node.hpp"

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
class RRStarTree : public BasicRtree<Node<D, C, CapturingEntryPlugin>, m>
{
	public:

		using N = Node<D, C, CapturingEntryPlugin>;
		using NIt = typename N::iterator;
		using M = Mbr<D>;

		/**
		 * Provide a couple of extra statistics.
		 */
		StatsCollector collectStatistics() const override;


	private:

		// Keep track of some extra statistics
		unsigned long long perimeterSplits = 0;
		unsigned long long negativeGoals = 0;

		/**
		 * Find a suitable subtree for the entry.
		 *
		 * @param entry Entry to find location for
		 * @param node Node in which the subtree should be
		 */
		NIt chooseSubtree(
				BaseEntry<N>& parent,
				const Entry<N>& newEntry
			) override;


		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The old entry (with children)
		 * @param b The new entry (with the new child)
		 * @param level Level of entries
		 */
		void redistribute(BaseEntry<N>& a, BaseEntry<N>& b, unsigned level) override;
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
	stats["negative_goals"] = negativeGoals;
	return stats;
}


template<unsigned D, unsigned C, unsigned m>
typename RRStarTree<D, C, m>::NIt RRStarTree<D, C, m>::chooseSubtree(
		BaseEntry<N>& parent,
		const Entry<N>& newEntry
	)
{
	N& node = parent.getNode();

	// Construct set of covering entries
	CoveringSet<NIt> covering (node.begin(), node.end(), newEntry);

	if (!covering.empty()) {
		return covering.minBy(
				covering.allHasVolume() ? &M::volume : &M::perimeter
			);
	}

	// Create a sorted view of the children (by delta perimeter)
	ReferenceView<NIt> children (node.begin(), node.end());

	children.sort([&](typename N::reference a, typename N::reference b) {
			return a.getMbr().delta(&M::perimeter, newEntry.getMbr())
				< b.getMbr().delta(&M::perimeter, newEntry.getMbr());
		});


	// Return the first one if its overlap perimeter enlargement is 0
	if (
			std::all_of(
					children.begin() + 1, children.end(),
					[&](typename N::reference entry) {
						return children[0]->getMbr().deltaOverlap(
								entry.getMbr(),
								newEntry.getMbr(),
								&M::perimeter
							) == 0.0;
					}
				)
	) {
		return children[0];
	}

	// Construct and run CheckComp
	using EIt = typename decltype(children)::iterator;
	CheckComp<EIt> checkComp (
			children.begin(),
			children.end(),
			newEntry
		);

	auto result = checkComp(children.begin());

	if (result != children.end()) {
		return *result.unwrap();
	}

	return *checkComp.minOverlap().unwrap();
};


template<unsigned D, unsigned C, unsigned m>
void RRStarTree<D, C, m>::redistribute(BaseEntry<N>& a, BaseEntry<N>& b, unsigned level)
{
	// Functions for evaluating splits
	GoalFunction wg (a.getMbr() + b.getMbr());
	WeightingFunction<N, m> wf (a);

	// Construct set of possible splits
	SplitSet<N, m> splits (
			a.getNode().begin(), b.getNode().begin(),
			a.getNode().end(), b.getNode().end()
		);


	// Restrict to single dimension for leafs
	if (level == 0) {

		unsigned dimension = *argmin(
				makeRangeIt(0u), makeRangeIt(D),
				[&](unsigned d) {
					splits.restrictTo(d);

					// Calculate the sum of all perimeters
					return std::accumulate(
							splits.begin(), splits.end(),
							0.0,
							[](double sum, const Split<N>& split) {
								return sum + split.perimeter();
							}
						);
				}
			);

		splits.restrictTo(dimension);
	}

	// Can we use volume?
	bool useVolume = std::all_of(
			splits.begin(), splits.end(),
			[](const Split<N>& split) {
				return split.hasVolume();
			}
		);

	if (!useVolume) {
		perimeterSplits++;
	}

	// Determine best split
	Split<N> split = *argmin(
			splits.begin(), splits.end(),
			[&](const Split<N>& split) {

				// Evaluate goal and weight functions
				wf.setDimension(split.getDimension());
				double g = wg(split, useVolume);
				double f = wf(split.getSplitPoint());

				return g < 0.0 ? g * f : g / f;
			}
		);

	if (wg(split, useVolume) < 0.0) {
		negativeGoals++;
	}

	// Distribute entries
	auto partitions = split.getEntries();
	a.getNode().assign(partitions[0].begin(), partitions[0].end());
	b.getNode().assign(partitions[1].begin(), partitions[1].end());

	a.recalculate();
	b.recalculate();
}

}
