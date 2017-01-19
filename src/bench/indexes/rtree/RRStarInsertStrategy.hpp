#pragma once
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <set>
#include "SpatialIndex.hpp"
#include "common/Algorithm.hpp"


namespace Rtree
{

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



class RRStarInsertStrategy
{
	public:

		static constexpr float EPSILON = 0.00001f;

		/**
		 * Insert an entry in the tree.
		 *
		 * @param object DataObject to insert
		 */
		template<class I>
		static void insert(I& index, const typename I::E& entry)
		{
			using E = typename I::E;

			E rootEntry (index.getRoot(), typename E::M());
			std::vector<E *> path {&rootEntry};

			// Find leaf node
			for (unsigned i = 0; i < index.getHeight() - 1; i++) {
				E& e = chooseSubtree(*path.back(), entry);
				e.mbr += entry.mbr;
				path.push_back(&e);
			}

			// Split nodes bottom-up as long as necessary
			E e = entry;
			auto top = path.rbegin();

			while (top != path.rend() && (*(top))->node->isFull()) {
				e = E(index.allocateNode(), {e});
				redistribute(**top, e, top == path.rbegin());
				top++;
			}

			// Split root?
			if (top == path.rend()) {
				E newRoot (index.allocateNode(), {**path.begin(), e});
				index.addLevel(newRoot.node);
			} else {
				(*top)->add(e);
			}
		};


	private:

		/**
		 * Extracts the most distant entries from the given entry.
		 *
		 * @param parent Parent from who's children entries should be extracted
		 * @param newEntry Extra entry to include
		 *
		 * @return Extracted entries ordered bycenter distance to parent
		 */
		template<class E>
		static std::vector<E> extractEntries(E& parent, const E& newEntry)
		{
			unsigned p = E::capacity - E::capacity/2;

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

			assert(p < E::capacity);
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
		 */
		template<class E>
		static E& chooseSubtree(E& parent, const E& newEntry)
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

				// If so, minimize perimeter
				if (noVolume) {
					return **argmin(
							covering.begin(),
							covering.end(),
							[](E * const & entry) {
								return entry->mbr.perimeter();
							}
						);
				}

				// Otherwise, select based on volume
				return **argmin(
						covering.begin(),
						covering.end(),
						[](E * const & entry) {
							return entry->mbr.volume();
						}
					);
			}

			// Create a sorted view of the children
			std::vector<E *> children;

			std::transform(
					parent.begin(),
					parent.end(),
					std::back_inserter(children),
					[](E& entry) {
						return &entry;
					}
				);

			std::sort(
					children.begin(),
					children.end(),
					[&](E * const & a, E * const & b) {
						return (a->mbr + newEntry.mbr).perimeter()
								- a->mbr.perimeter()
							< (b->mbr + newEntry.mbr).perimeter()
								- b->mbr.perimeter();
					}
				);


			// For some weired reason, return the first one if its perimeter
			// enlargement is 0 for the new entry. (optimization)
			if (
					perimeterOverlap(parent, children[0]->mbr + newEntry.mbr)
					- perimeterOverlap(parent, newEntry.mbr) < EPSILON
			) {
				return *children[0];
			}

			auto p = argmin(
					children.begin(),
					children.end(),
					[&](E * const & child) {
						return -(
							perimeterOverlap(parent, child->mbr + newEntry.mbr)
							- perimeterOverlap(parent, newEntry.mbr)
						);
					}
				) - children.begin() + 1;

			//TODO: Combined calculation like they do in the paper?
			struct Frame {
				unsigned index;
				unsigned j = 0;
				Frame(unsigned i) : index(i) {};
			};

			std::set<unsigned> visited;
			std::vector<float> overlaps (p, 0.0f);

			std::stack<Frame> path;
			path.emplace(0);
			visited.emplace(0);

			using M = typename E::M;


			while (path.size()) {
				unsigned j = path.top().j++;

				// Done with this node yet?
				if (j >= p) {
					if (overlaps[path.top().index] < EPSILON) {
						return *children[path.top().index];
					}

					path.pop();
					continue;
				}

				E * child = children[j];

				// Skip the same node
				if (j == path.top().index) {
					continue;
				}

				// Calculate overlap (TODO: Support perimeter)
				const M& top = children[path.top().index]->mbr;
				const M enlarged = top + newEntry.mbr;
				const M& other = child->mbr;

				if (!other.intersects(enlarged)) {
					continue;
				}

				float overlap;
				if (other.intersects(top)) {
					overlap = enlarged.intersection(other).volume()
						- top.intersection(other).volume();
				} else {
					overlap = enlarged.intersection(other).volume();
				}

				// Skip if not "connected" or already visited
				if (overlap < EPSILON || visited.count(j)) {
					continue;
				}

				// "Recursive call"
				overlaps[j] += overlap;
				path.emplace(j);
				visited.emplace(j);
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
		template<class E>
		static float perimeterOverlap(E& parent, typename E::M mbr)
		{
			return overlap(
					parent,
					mbr,
					[](const typename E::M& intersection) {
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
		template<class E, class F>
		static float overlap(E& parent, typename E::M mbr, F type)
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


		template<class ForwardIt>
		static float evaluateSplit(
				ForwardIt begin,
				ForwardIt middle,
				ForwardIt end,
				float maxPerimeter,
			   	bool useVolume = true
		) {
			using E = typename ForwardIt::value_type;
			using M = typename E::M;

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
		}



		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The first entry (with children)
		 * @param b The second entry (with children)
		 */
		template<class E>
		static void redistribute(E& a, E& b, bool isLeaf = false) //TODO: isLeaf
		{
			// Contruct buffer with all entries
			std::vector<E> entries (a.begin(), a.end());

			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);

			const unsigned m = E::capacity / 4;
			using M = typename E::M;

			// This is assumed
			assert(entries.size() > 2 * m);

			// Calculate max perimeter
			M all = entries.front().mbr;
			for (const E& e : entries) {
				all += e.mbr;
			}

			float maxPerimeter = all.perimeter();


			// To be updated
			unsigned bestDimension = E::dimension;
			unsigned bestSplit = E::capacity;

			if (isLeaf) {
				// Select an axis
				bestDimension = *argmin(
						makeRangeIt(0u), makeRangeIt(E::dimension),
						[&](unsigned i) {
							sortBy(i, entries.begin(), entries.end());

							float minPerimeter = std::numeric_limits<float>::infinity();

							for (unsigned s = m; s < entries.size() - m; ++s) {

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
								float perimeter = mbrA.perimeter() + mbrB.perimeter();

								if (perimeter < minPerimeter) {
									minPerimeter = perimeter;
								}
							}

							return minPerimeter;
						}
					);

				// Sort along current dimension
				sortBy(bestDimension, entries.begin(), entries.end());

				// Is volume of every split candidate > 0?
				bool hasVolume = std::all_of(
						entries.begin() + m, entries.end() - m,
						[](const E& e) { return e.mbr.volume() >= EPSILON; }
					);

				// Select split point
				bestSplit = *argmin(
						makeRangeIt(m), makeRangeIt((unsigned) entries.size() - m),
						[&entries, &hasVolume, &maxPerimeter](unsigned s) {
							//TODO: weighting
							return evaluateSplit(
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

					for (unsigned s = m; s < entries.size() - m; ++s) {
						//TODO: weighting
						float w = evaluateSplit(
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
			assert(bestSplit < E::capacity);

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
