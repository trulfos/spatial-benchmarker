#pragma once
#include <set>
#include <algorithm>
#include <functional>
#include "Algorithm.hpp"
#include "Entry.hpp"

namespace Rtree
{

/**
 * A set of entries covering a new entry (not part of the set).
 */
template<class It>
class CoveringSet : std::set<It>
{

	using Base = std::set<It>;

	// Depends on template parameters
	using Base::begin;
	using Base::end;
	using Base::insert;

	public:

		/**
		 * Create a new covering set by filtering the given entries.
		 */
		template<class N>
		CoveringSet(It first, It last, const Entry<N>& entry)
		{
			while (first != last) {
				if (first->getMbr().contains(entry.getMbr())) {
					insert(first);
				}

				++first;
			}
		}


		/**
		 * Check if this set is empty.
		 */
		using Base::empty;


		/**
		 * Check that none of the entries in th set lacks volume.
		 */
		bool allHasVolume()
		{
			return std::all_of(
						begin(), end(),
						[](It entry) {
							return entry->getMbr().volume() != 0.0;
						}
					);
		}

		template<class M>
		It minBy(double (M::*measure)() const)
		{
			return *argmin(
					begin(), end(),
					[&](It entry) {
						return (entry->getMbr().*measure)();
					}
				);
		}

};

}
