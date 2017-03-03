#pragma once
#include <iterator>
#include <utility>
#include "common/Algorithm.hpp"
#include "CombinationsView.hpp"

namespace Rtree
{

/**
 * Generates the seeds used for the quadratic R-tree.
 */
template<class FIt>
class QuadraticSeeds : public std::pair<FIt, FIt>
{
	public:
		QuadraticSeeds(FIt first, FIt last);
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
QuadraticSeeds<FIt>::QuadraticSeeds(FIt first, FIt last)
{
	using E = typename std::iterator_traits<FIt>::value_type;

	// Choose the two seeds by checking all combinations
	auto combinations = makeCombinationsView(first, last);

	auto bestCombination = argmin(
			combinations.begin(), combinations.end(),
			[](std::pair<const E&, const E&> pair) {
				return -pair.first.mbr.waste(pair.second.mbr);
			}
		);

	*static_cast<std::pair<FIt, FIt> *>(this) = bestCombination.getIterators();
}

}
