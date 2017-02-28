#pragma once
#include <iterator>
#include <vector>
#include <set>
#include <algorithm>

/**
 * Implements the CheckComp function from the RR*-tree paper.
 */
template<class FIt, class E>
class CheckComp
{
	//using E = typename std::iterator_traits<FIt>::value_type;
	using M = typename E::M;

	public:
		CheckComp(FIt first, FIt last, const E& newEntry)
			: first(first), last(last), overlaps(last - first, 0.0), newEntry(newEntry)
		{
			useVolume = std::all_of(
					first, last,
					[&](const E& e) {
						return (e.mbr + newEntry.mbr).volume() != 0.0;
					}
				);
		}

		FIt operator()()
		{
			FIt r = implementation(first);

			if (r != last) {
				return r;
			}

			return *argmin(
						visited.begin(),
						visited.end(),
						[&](FIt i) {
							return overlaps[i - first];
						}
					);
		}

	private:
		FIt first, last;
		std::vector<double> overlaps; // <delta>ovlp
		E newEntry;

		bool useVolume;
		std::set<FIt> visited; // CAND


		FIt implementation(FIt t)
		{
			visited.emplace(t);

			for (FIt j = first; j != last; ++j) {
				if (j == t) {
					continue;
				}

				// Calculate overlap enlargement
				double overlap = t->mbr.overlapEnlargement(
						j->mbr,
						newEntry.mbr,
						useVolume? &M::volume : &M::perimeter
					);

				overlaps[t - first] += overlap;

				// Descend if overlapping and not visited
				if (overlap != 0.0 && !visited.count(j)) {
					FIt r = implementation(j);
					if (r != last){
						return r;
					}
				}
			}

			return overlaps[t - first] == 0.0 ? t : last;
		}
};

