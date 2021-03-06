#pragma once
#include <vector>
#include <numeric>
#include <algorithm>
#include <memory>
#include "ReferenceView.hpp"
#include "Entry.hpp"

namespace Rtree
{

/**
 * Represents a possible split of a set of entries used by the RR*-tree.
 */
template<class N>
class Split
{
	using NIt = typename N::iterator;

	public:

		using Mbr = typename N::Mbr;

		Split(
				const std::vector<const Entry<N> *>& entryView,
				unsigned sort,
				unsigned dimension,
				unsigned splitPoint
			)
			: sort(sort), dimension(dimension), splitPoint(splitPoint),
				entryView(entryView)
		{
			assert(splitPoint < entryView.size());

			auto middle = entryView.begin() + splitPoint;
			mbrs = {{
					mbrSum(entryView.begin(), middle),
					mbrSum(middle, entryView.end())
				}};
		}

		/**
		 * Retrieve MBRs for the two parts
		 */
		const std::array<Mbr, 2>& getMbrs() const
		{
			return mbrs;
		}


		using EVec = std::vector<Entry<N>>;

		/**
		 * Retrieve the entries contained in each part.
		 *
		 * TODO: Avoid copying the entrie view by forwarding this request to the
		 * iterator itself (triggering a new sort or special copy operation).
		 */
		std::array<EVec, 2> getEntries() const
		{
			auto middle = entryView.begin() + splitPoint;

			return {{
					EVec(makeDerefIt(entryView.begin()), makeDerefIt(middle)),
					EVec(makeDerefIt(middle), makeDerefIt(entryView.end()))
				}};
		}

		unsigned getDimension() const
		{
			return dimension;
		}

		unsigned getSort() const
		{
			return sort;
		}

		unsigned getSplitPoint() const
		{
			return splitPoint;
		}

		double perimeter() const
		{
			return mbrs[0].perimeter() + mbrs[1].perimeter();
		}


		bool hasVolume() const
		{
			return mbrs[0].volume() != 0.0 && mbrs[1].volume() != 0.0;
		}

	private:
		unsigned sort, dimension, splitPoint;
		std::array<Mbr, 2> mbrs;
		std::vector<const Entry<N> *> entryView;

		/**
		 * Sums up MBRs.
		 */
		template<class FIt>
		static Mbr mbrSum(FIt first, FIt last)
		{
			assert(first != last);
			using E = typename std::iterator_traits<FIt>::value_type;

			return std::accumulate(
					first, last,
					(*first)->getMbr(),
					[](const Mbr& sum, const E e) {
						return sum + e->getMbr();
					}
				);
		}
};

}
