#include <criterion/criterion.h>
#include "CombinationsView.hpp"
#include "Range.hpp"


Test(CombinationsView, basic)
{
	unsigned
		start = 5,
		end = 20;

	CombinationsView<RangeIterator<unsigned>> combinations (start, end);

	auto it = combinations.begin();

	for (unsigned i = start; i < end; ++i) {
		for (unsigned j = i + 1; j < end; ++j) {
			cr_assert_eq(
					*it,
					decltype(combinations)::value_type(i, j)
				);

			cr_assert(it != combinations.end());

			++it;
		}
	}

	cr_assert(it == combinations.end());

}
