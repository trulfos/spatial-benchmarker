#pragma once
#include <vector>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <memory>

/**
 * Represents a possible split of a set of entries used by the RR*-tree.
 */
template<class E>
class Split
{
	using M = typename E::M;

	public:

		Split(
				const std::vector<E *>& entryView,
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
		const std::array<M, 2>& getMbrs() const
		{
			return mbrs;
		}


		/**
		 * Retrieve the entries contained in each part.
		 *
		 * TODO: Avoid copying the entrie view by forwarding this request to the
		 * iterator itself (triggering a new sort or special copy operation).
		 */
		std::array<std::vector<E>, 2> getEntries() const
		{
			auto middle = entryView.begin() + splitPoint;

			std::vector<E>
				low(
						makeDerefIt(entryView.begin()),
						makeDerefIt(middle)
					),
				high (
						makeDerefIt(middle),
						makeDerefIt(entryView.end())
					);

			return {{low, high}};
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

	private:
		unsigned sort, dimension, splitPoint;
		std::array<M, 2> mbrs;
		std::vector<E *> entryView;

		/**
		 * Sums up MBRs.
		 */
		template<class FIt>
		static M mbrSum(FIt first, FIt last)
		{
			assert(first != last);

			return std::accumulate(
					first, last,
					(*first)->mbr,
					[](const M& sum, const E* e) {
						return sum + e->mbr;
					}
				);
		}
};
