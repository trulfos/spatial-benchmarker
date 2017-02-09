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

		static constexpr float EPSILON = 0.00001f;


		/**
		 * Insert an entry in the tree.
		 *
		 * @param object DataObject to insert
		 */
		virtual void insert(const E& entry) override
		{
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
			E e = entry;
			auto top = path.rbegin();

			while (top != path.rend() && (*(top))->node->isFull()) {
				e = E(this->allocateNode(), {e});
				redistribute(**top, e, top == path.rbegin());
				top++;
			}

			// Split root?
			if (top == path.rend()) {
				E newRoot (this->allocateNode(), {**path.begin(), e});
				this->addLevel(newRoot.node);
			} else {
				(*top)->add(e);
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
			std::vector<E *> covering;

			for (E& entry : parent) {
				if (entry.mbr.contains(newEntry.mbr)) {
					covering.push_back(&entry);
				}
			}

			if (covering.size()) {

				// Determine if any of the entries lack volume
				bool noVolume = std::any_of(
						covering.begin(),
						covering.end(),
						[](E * const & entry) {
							return entry->mbr.volume() < EPSILON;
						}
					);

				// Minimize perimeter/volume
				return **argmin(
						covering.begin(),
						covering.end(),
						[&](E * const & entry) {
							return noVolume ?
								entry->mbr.perimeter() : entry->mbr.volume();
						}
					);
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
					- perimeterOverlap(parent, newEntry.mbr) < EPSILON
			) {
				return *children[0];
			}

			// Determine threshold (optimization)
			auto p = argmin(
					children.begin() + 1, children.end(),
					[&](E * const & child) {
						return children.front()->mbr.overlapEnlargement(
								child->mbr,
								newEntry.mbr,
								[](const M& mbr) { return mbr.perimeter(); }
							);
					}
				) - children.begin() + 1;

			// Determine whether volume or perimeter should be used
			bool useVolume = std::all_of(
					children.begin(), children.begin() + p,
					[&](const E * e) { return e->mbr.intersects(newEntry.mbr); }
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

			std::set<unsigned> visited;
			std::vector<float> overlaps (p, 0.0f);

			std::stack<StackFrame> path;
			path.emplace(0);
			visited.emplace(0);


			while (path.size()) {
				unsigned current = path.top().index;
				unsigned j = path.top().j++;

				// Done with this node yet?
				if (j >= p) {

					// We may have found what we are looking for
					if (overlaps[current] < EPSILON) {
						return *children[current];
					}

					path.pop();
					continue;
				}

				// Skip current node
				if (j == current) {
					continue;
				}

				E * child = children[j];

				// Calculate overlap enlargement
				float overlap = children[current]->mbr.overlapEnlargement(
						child->mbr,
						newEntry.mbr,
						measure
					);

				// Descend if overlapping and not visited
				if (overlap > EPSILON && !visited.count(j)) {
					overlaps[j] += overlap;
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
		float perimeterOverlap(E& parent, M mbr)
		{
			return overlap(
					parent,
					mbr,
					[](const M& intersection) {
						return intersection.perimeter();
					}
				);
		};


		/**
		 * Calculate the overlap between an MBR and the children of an entry's
		 * node.
		 *
		 * @param parent Parent entry who's children will be used
		 * @param mbr MBR to calculate overlap for
		 *
		 * @return Overlap of the MBR with the children
		 */
		template<class F>
		float overlap(E& parent, M mbr, F type)
		{
			return std::accumulate(
					parent.begin(),
					parent.end(),
					0.0f,
					[&](const float& sum, const E& entry) {
						return mbr.intersects(entry.mbr) ?
							sum + type(mbr.intersection(entry.mbr)) : sum;
					}
				);
		};


		/**
		 * Sort the given entries by their projection in the given dimension.
		 * TODO: Move this
		 */
		template<class ForwardIt>
		void sortBy(unsigned dimension, ForwardIt begin, ForwardIt end)
		{
			using E = typename ForwardIt::value_type;
			std::sort(
					begin, end,
					[&](const E& a, const E& b) {

						auto diff = a.mbr.getBottom()[dimension]
							- b.mbr.getBottom()[dimension];

						if (diff == 0.0f) {
							return diff < 0.0f;
						}

						return a.mbr.getTop()[dimension]
							< b.mbr.getTop()[dimension];
					}
				);
		}



		template<class ForwardIt>
		float evaluateSplit(
				ForwardIt begin,
				ForwardIt middle,
				ForwardIt end,
				float maxPerimeter,
			   	bool useVolume = true
		) {
			assert(end - begin > 0);

			// Sum up MBRs (again)
			M mbrA = std::accumulate(
					begin, middle,
					begin->mbr,
					[](const M& sum, const E& e) { return sum + e.mbr; }
				);

			M mbrB = std::accumulate(
					middle, end,
					(end - 1)->mbr,
					[](const M& sum, const E& e) { return sum + e.mbr; }
				);

			// Return overlap (if any)
			if (mbrA.intersects(mbrB)) {
				const M intersection = mbrA.intersection(mbrB);

				float overlap = useVolume ?
					intersection.volume() : intersection.perimeter();

				if (overlap > EPSILON) {
					return overlap;
				}
			}

			// Otherwise, use (negative) perimeter
			return mbrA.perimeter()
				+ mbrB.perimeter()
				- maxPerimeter;
		};


		/**
		 * Find the minimal split perimeter along the given axis.
		 */
		float minSplitPerimeter(unsigned d, std::vector<E>& entries)
		{
			// Sort along the given dimension
			sortBy(d, entries.begin(), entries.end());


			return min_value(
					makeRangeIt(m), makeRangeIt(unsigned(entries.size()) - m),
					[&](const unsigned s) {

						// Generate MBRs for the two groups
						M mbrA = entries.front().mbr;
						M mbrB = entries.back().mbr;

						for (unsigned i = 1; i < s; ++i) {
							mbrA += entries[i].mbr;
						}

						for (unsigned i = s; i < entries.size() - 1; ++i) {
							mbrB += entries[i].mbr;
						}

						// Calculate perimeter (for dimension selection)
						return mbrA.perimeter() + mbrB.perimeter();

					}
				);
		}


		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The old entry (with children)
		 * @param b The new entry (with the new child)
		 */
		void redistribute(E& a, E& b, bool isLeaf = false) //TODO: isLeaf
		{
			// Contruct buffer with all entries
			std::vector<E> entries (a.begin(), a.end());

			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);

			// This is assumed
			assert(entries.size() > 2 * m);

			// Calculate max perimeter
			M all = entries.front().mbr;
			for (const E& e : entries) {
				all += e.mbr;
			}

			float maxPerimeter = all.perimeter();
			WeightingFunction<E, m> wf(a);

			// To be updated
			unsigned bestDimension = E::dimension;
			unsigned bestSplit = E::Node::capacity;

			if (isLeaf) {
				// Select an axis
				bestDimension = *argmin(
						makeRangeIt(0u), makeRangeIt(E::dimension),
						[&](unsigned i) {
							return minSplitPerimeter(i, entries);
						}
					);

				// Sort along current dimension
				sortBy(bestDimension, entries.begin(), entries.end());

				// Is volume of every split candidate > 0?
				bool hasVolume = std::all_of(
						entries.begin() + m, entries.end() - m,
						[](const E& e) { return e.mbr.volume() >= EPSILON; }
					);

				wf.setDimension(bestDimension);

				// Select split point
				bestSplit = *argmin(
						makeRangeIt(m), makeRangeIt((unsigned) entries.size() - m),
						[&](unsigned s) {

							return wf(s) * evaluateSplit(
									entries.begin(),
									entries.begin() + s,
									entries.end(),
									maxPerimeter,
									hasVolume
								);
						}
					);

			} else { // Not leaf case

				// Use all axes
				float minimum = std::numeric_limits<float>::infinity();

				for (unsigned d = 0; d < E::dimension; ++d) {
					// Sort along current dimension
					sortBy(d, entries.begin(), entries.end());

					// Is volume of every split candidate > 0?
					bool hasVolume = std::all_of(
							entries.begin() + m, entries.end() - m,
							[](const E& e) {
								return e.mbr.volume() >= EPSILON;
							}
						);

					wf.setDimension(d);

					for (unsigned s = m; s < entries.size() - m; ++s) {
						//TODO: weighting
						float w = wf(s) * evaluateSplit(
								entries.begin(),
								entries.begin() + s,
								entries.end(),
								maxPerimeter,
								hasVolume
							);

						if (w < minimum) {
							minimum = w;
							bestSplit = s;
							bestDimension = d;
						}
					}
				}
			}

			assert(bestDimension < E::dimension);
			assert(bestSplit < E::Node::capacity);

			// Distribute entries
			auto middle = entries.begin() + bestSplit;

			std::nth_element(
					entries.begin(),
					middle,
					entries.end(),
					[&](const E& a, const E& b) {

						//TODO: Is this right?
						auto diff = a.mbr.getBottom()[bestDimension]
							- b.mbr.getBottom()[bestDimension];

						if (diff == 0.0f) {
							return diff < 0.0f;
						}

						return a.mbr.getTop()[bestDimension]
							< b.mbr.getTop()[bestDimension];
					}
				);

			a.assign(entries.begin(), middle);
			b.assign(middle, entries.end());

			// Update center positions
			a.node->captureMbr();
			b.node->captureMbr();
		};
};

}
