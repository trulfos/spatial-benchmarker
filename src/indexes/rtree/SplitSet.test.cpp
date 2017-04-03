#include <criterion/criterion.h>
#include "SplitSet.hpp"
#include "Entry.hpp"
#include "spatial/DataObject.hpp"

using namespace Rtree;

class Node {};
using E = Entry<2, Node>;

// Create a set of entries for testing
std::array<const E, 5> entries = {{
		DataObject(1, {Point {0.0, 4.0}, Point {6.0, 8.0}}),
		DataObject(2, {Point {1.0, 3.0}, Point {7.0, 6.0}}),
		DataObject(3, {Point {2.0, 2.0}, Point {9.0, 7.0}}),
		DataObject(4, {Point {3.0, 1.0}, Point {8.0, 9.0}}),
		DataObject(5, {Point {4.0, 0.0}, Point {5.0, 5.0}})
	}};

// Correct sort orders
std::array<std::array<unsigned, 5>, 4> orders = {{
		{{5, 1, 2, 4, 3}},
		{{1, 2, 3, 4, 5}},
		{{5, 2, 3, 1, 4}},
		{{5, 4, 3, 2, 1}}
	}};


/**
 * Test basic iteration and the values of dimension, split and sort.
 */
Test(SplitSet, basic)
{
	// Create the split set
	SplitSet<const E, 1> splits(
			entries.begin(), entries.begin(),
			entries.end(), entries.begin()
		);

	// Loop through all combinations
	auto first = splits.begin();

	for (unsigned d = 0; d < 2; ++d) {
		for (unsigned s = 0; s < 2; ++s) {
			for (unsigned p = 0; p < 4; ++p) {
				cr_expect_eq(d, first->getDimension());
				cr_expect_eq(s, first->getSort());
				cr_expect_eq(p + 1, first->getSplitPoint());
				cr_expect(first != splits.end());

				++first;
			}
		}
	}

	cr_assert(first == splits.end());
}


/**
 * Test the ordering of the resulting entries.
 */
Test(SplitSet, ordering)
{
	// Create the split set
	SplitSet<const E, 1> splits(
			entries.begin(), entries.begin(),
			entries.end(), entries.begin()
		);

	for (auto& s : splits) {
		auto parts = s.getEntries();
		unsigned i = 0;

		for (auto part : parts) {
			for (auto entry : part) {
				cr_expect_eq(
						entry.getId(),
						orders[2 * s.getDimension() + s.getSort()][i]
					);
				i++;
			}
		}
	}
}


/**
 * Test the restrictTo method.
 */
Test(SplitSet, restrictTo)
{
	// Create the split set
	SplitSet<const E, 1> splits(
			entries.begin(), entries.begin(),
			entries.end(), entries.begin()
		);

	for (unsigned d = 0; d < 2; ++d) {
		// Restrict!
		splits.restrictTo(1 - d);

		// Loop through all combinations
		auto first = splits.begin();

		for (unsigned s = 0; s < 2; ++s) {
			for (unsigned p = 0; p < 4; ++p) {
				cr_expect_eq(1 - d, first->getDimension());
				cr_expect_eq(s, first->getSort());
				cr_expect_eq(p + 1, first->getSplitPoint());
				cr_expect(first != splits.end());

				++first;
			}
		}

		cr_expect(first == splits.end());
	}
}
