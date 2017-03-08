#pragma once
#include <set>
#include <algorithm>
#include <functional>
#include "Algorithm.hpp"

/**
 * A set of entries covering a new entry (not part of the set).
 */
template<class E>
class CoveringSet : std::set<E *>
{
	public:

		/**
		 * Create a new covering set by filtering the given entries.
		 */
		template<class FIt>
		CoveringSet(FIt first, FIt last, const E& entry)
		{
			while (first != last) {
				if (first->mbr.contains(entry.mbr)) {
					this->insert(&*first);
				}

				++first;
			}
		}


		/**
		 * Check if this set is empty.
		 */
		using std::set<E *>::empty;


		/**
		 * Check that none of the entries in th set lacks volume.
		 */
		bool allHasVolume()
		{
			return std::all_of(
						this->begin(),
						this->end(),
						[](const E * const & entry) {
							return entry->mbr.volume() != 0.0;
						}
					);
		}

		E& minBy(double (E::M::*measure)() const)
		{
			return **argmin(
					this->begin(),
					this->end(),
					[&](const E * entry) {
						return (entry->mbr.*measure)();
					}
				);
		}

};
