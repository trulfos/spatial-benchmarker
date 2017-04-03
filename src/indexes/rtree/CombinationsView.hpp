#pragma once
#include <iterator>
#include <utility>
#include <cassert>

namespace Rtree
{

/**
 * Gives the combinations possible by combining a range with itself.
 */
template<class FIt>
class CombinationsView
{
	public:

		using value_type = std::pair<
				typename std::iterator_traits<FIt>::reference,
				typename std::iterator_traits<FIt>::reference
			>;

		class CombinationsIterator
			: public std::iterator<std::forward_iterator_tag, value_type>

		{
			public:
				CombinationsIterator(const CombinationsView * view, FIt current)
					: first(current), second(current), view(view)
				{
					if (current != view->last) {
						operator++();
					}
				}

				CombinationsIterator operator++()
				{
					assert(
							first != view->last &&
							second != view->last
						);

					++second;

					if (second == view->last) {
						++first;
						second = first;
						++second;

						if (second == view->last) {
							++first;
						}
					}

					assert(
							(first == view->last) == (second == view->last)
						);

					return *this;
				}

				value_type operator*()
				{
					return value_type(*first, *second);
				}

				bool operator==(const CombinationsIterator& other) const
				{
					return view == other.view &&
						first == other.first &&
						second == other.second;
				}

				bool operator!=(const CombinationsIterator& other) const
				{
					return !(*this == other);
				}

				std::pair<FIt, FIt> getIterators()
				{
					return std::make_pair(first, second);
				}

			private:
				FIt first, second;
				const CombinationsView * view;
		};

		using iterator = CombinationsIterator;

		CombinationsView(FIt first, FIt last) : first(first), last(last)
		{
		}

		iterator begin() const
		{
			return CombinationsIterator(this, first);
		}

		iterator end() const
		{
			return CombinationsIterator(this, last);
		}

	private:
		FIt first;
		FIt last;
};

template<class FIt>
CombinationsView<FIt> makeCombinationsView(FIt first, FIt last)
{
	return CombinationsView<FIt>(first, last);
}

}
