#pragma once
#include <iterator>
#include <vector>
#include <set>
#include <algorithm>
#include "Algorithm.hpp"
#include "Entry.hpp"

namespace Rtree
{

/**
 * Implements the CheckComp function from the RR*-tree paper.
 */
template<class FIt>
class CheckComp
{
	using E = typename std::iterator_traits<FIt>::value_type;
	using N = typename E::Node;
	using M = typename E::Mbr;

	public:
		/**
		 * Construct a new environment for the computation with the range of
		 * entries given.
		 */
		CheckComp(FIt first, FIt last, const Entry<N>& newEntry);


		/**
		 * Execute the checkComp procedure.
		 */
		FIt operator()(FIt t);


		/**
		 * Finds the entry with minimum overlap enlargement as calculated during
		 * traversal.
		 */
		FIt minOverlap();


		/**
		 * Get the threshold p.
		 */
		FIt getP() const;


		/**
		 * Get visited entries.
		 */
		std::set<FIt> getVisited() const;

	private:
		FIt first, last, p;
		std::vector<double> overlaps; // <delta>ovlp
		std::set<FIt> visited; // CAND

		const Entry<N>& newEntry;
		bool useVolume;
};


/*

 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
*/
template<class FIt>
CheckComp<FIt>::CheckComp(FIt first, FIt last, const Entry<N>& newEntry)
	: first(first), last(last), p(first), overlaps(last - first, 0.0),
		newEntry(newEntry)
{
	assert(first != last);

	useVolume = std::all_of(
			first, last,
			[&](const Entry<N>& e) {
				return (e.getMbr() + newEntry.getMbr()).volume() != 0.0;
			}
		);

	// Determine threshold p (optimization)
	p = first + 1;

	for (auto i = p; i != last; ++i) {
		double deltaOvlp = first->getMbr().deltaOverlap(
				i->getMbr(),
				newEntry.getMbr(),
				&M::perimeter
			);

		if (deltaOvlp > 0.0) {
			p = i + 1;
		}
	}

}


template<class FIt>
FIt CheckComp<FIt>::operator()(FIt t)
{
	visited.emplace(t);

	for (FIt j = first; j != p; ++j) {
		if (j == t) {
			continue;
		}

		// Calculate overlap enlargement
		double overlap = t->getMbr().deltaOverlap(
				j->getMbr(),
				newEntry.getMbr(),
				useVolume? &M::volume : &M::perimeter
			);

		overlaps[t - first] += overlap;

		// Descend if overlapping and not visited
		if (overlap != 0.0 && !visited.count(j)) {
			FIt r = operator()(j);
			if (r != last){
				return r;
			}
		}
	}

	return overlaps[t - first] == 0.0 ? t : last;
}


template<class FIt>
FIt CheckComp<FIt>::minOverlap()
{
	return *argmin(
				visited.begin(),
				visited.end(),
				[&](FIt i) {
					return overlaps[i - first];
				}
			);
}

template<class FIt>
FIt CheckComp<FIt>::getP() const
{
	return p;
}

template<class FIt>
std::set<FIt> CheckComp<FIt>::getVisited() const
{
	return visited;
}

}
