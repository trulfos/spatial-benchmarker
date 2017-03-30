#pragma once
#include <vector>
#include <numeric>
#include <algorithm>
#include <memory>
#include "ReferenceView.hpp"

/**
 * Represents a possible split of a set of entries used by the RR*-tree.
 */
template<class E>
class Split
{
	using M = typename E::M;

	public:

		Split(
				const ReferenceView<E>& entryView,
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


		using rvec = std::vector<
				typename std::remove_const<E>::type
			>;

		/**
		 * Retrieve the entries contained in each part.
		 *
		 * TODO: Avoid copying the entrie view by forwarding this request to the
		 * iterator itself (triggering a new sort or special copy operation).
		 */
		std::array<rvec, 2> getEntries() const
		{
			auto middle = entryView.begin() + splitPoint;

			return {{
					rvec(entryView.begin(), middle),
					rvec(middle, entryView.end())
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
		std::array<M, 2> mbrs;
		ReferenceView<E> entryView;

		/**
		 * Sums up MBRs.
		 */
		template<class FIt>
		static M mbrSum(FIt first, FIt last)
		{
			assert(first != last);

			return std::accumulate(
					first, last,
					first->getMbr(),
					[](const M& sum, const E& e) {
						return sum + e.getMbr();
					}
				);
		}
};
