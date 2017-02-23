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


	private:

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
			std::vector<E *> children;

			std::transform(
					parent.begin(), parent.end(),
					std::back_inserter(children),
					[](E& entry) {
						return &entry;
					}
				);

			std::sort(
					children.begin(), children.end(),
					[&](E * const & a, E * const & b) {
						return (a->mbr + newEntry.mbr).perimeter()
								- a->mbr.perimeter()
							< (b->mbr + newEntry.mbr).perimeter()
								- b->mbr.perimeter();
					}
				);


			// Optimize by returning the first one if its perimeter
			// enlargement is 0 for the new entry
			if (
					perimeterOverlap(parent, children[0]->mbr + newEntry.mbr)
					== perimeterOverlap(parent, newEntry.mbr)
			) {
				return *children[0];
			}

			// Determine threshold (optimization)
			unsigned p = children.size();

			for (unsigned i = p; i < children.size(); ++i) {
				double deltaOvlp = children.front()->mbr.overlapEnlargement(
						children[p]->mbr,
						newEntry.mbr,
						[](const M& mbr) { return mbr.perimeter(); }
					);

				if (deltaOvlp > 0.0) {
					p = i + 1;
				}
			}

			// Determine whether volume or perimeter should be used
			bool useVolume = !std::any_of(
					children.begin(), children.begin() + p,
					[&](const E * e) {
						return (e->mbr + newEntry.mbr).volume() == 0.0;
					}
				);

			auto measure = useVolume?
				[](const M& mbr) { return mbr.volume(); } :
					[] (const M& mbr) { return mbr.perimeter(); };


			// Start depth-first traversal
			struct StackFrame {
				unsigned index;
				unsigned j = 0;
				StackFrame(unsigned i) : index(i) {};
			};

			std::set<unsigned> visited; // CAND
			std::vector<double> overlaps (p, 0.0f); // <delta>ovlp

			std::stack<StackFrame> path;
			path.emplace(0);
			visited.emplace(0);


			while (path.size()) {
				unsigned current = path.top().index;
				unsigned j = path.top().j++;

				// Done with this node yet?
				if (j >= p) {

					// We may have found what we are looking for
					if (overlaps[current] == 0.0) {
						return *children[current];
					}

					path.pop();
					continue;
				}

				// Skip current node
				if (j == current) {
					continue;
				}

				// Calculate overlap enlargement
				double overlap = children[current]->mbr.overlapEnlargement(
						children[j]->mbr,
						newEntry.mbr,
						measure
					);

				overlaps[current] += overlap;

				// Descend if overlapping and not visited
				if (overlap != 0.0 && !visited.count(j)) {
					path.emplace(j);
					visited.emplace(j);
				}

			}

			unsigned bestIndex = *argmin(
						visited.begin(),
						visited.end(),
						[&](unsigned index) {
							return overlaps[index];
						}
					);

			return *children[bestIndex];

		};


		/**
		 * Calculate the perimeter overlap of an MBR with al the children of an
		 * entry's node.
		 *
		 * @param parent Parent entry who's children will be used
		 * @param mbr MBR to calculate overlap for
		 *
		 * @return Overlap of the MBR with the children
		 */
		double perimeterOverlap(E& parent, M mbr)
		{
			return std::accumulate(
					parent.begin(),
					parent.end(),
					0.0f,
					[&](const double& sum, const E& entry) {
						return mbr.intersects(entry.mbr) ?
							sum + mbr.intersection(entry.mbr).perimeter() : sum;
					}
				);
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

			auto first = splits.begin();


			// Restrict to single dimension for leafs
			if (isLeaf) {
				const Split<E>& split = *argmin(
							first, splits.end(),
							[](const Split<E>& split) {
								return split.perimeter();
							}
						);

				splits.restrictTo(split.getDimension());
			}

			// Can we use volume?
			// TODO: This can be optimized by iterating through sorts instead of
			//		splits.
			bool useVolume = std::all_of(
					first, splits.end(),
					[](const Split<E>& split) {
						return split.hasVolume();
					}
				);

			// Determine best split
			Split<E> split = *argmin(
					first, splits.end(),
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
