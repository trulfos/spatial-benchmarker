#include <criterion/criterion.h>
#include "QuadraticSeeds.hpp"
#include "Entry.hpp"
#include <algorithm>
#include <vector>

using namespace Rtree;

class N {};
using E = Entry<2, N>;

// Create a set of entries for testing
std::array<const E, 4> entries = {{
		DataObject(1, {Point {6.0, 5.0}, Point {7.0, 8.0}}),
		DataObject(2, {Point {3.0, 5.0}, Point {5.0, 7.0}}),
		DataObject(3, {Point {6.0, 3.0}, Point {7.0, 4.0}}),
		DataObject(4, {Point {2.0, 2.0}, Point {4.0, 4.0}})
	}};

using It = typename decltype(entries)::iterator;

Test(QuadraticSeeds, basic_example)
{
	std::array<std::pair<unsigned, unsigned>, 3> correct {{
		{0, 3}, {1, 2}, {2, 3}
	}};

	for (unsigned i = 0; i < correct.size(); ++i) {
		QuadraticSeeds<It> seeds (entries.begin() + i, entries.end());

		cr_assert_eq(
				seeds.first,
				entries.begin() + correct[i].first
			);

		cr_assert_eq(
				seeds.second,
				entries.begin() + correct[i].second
			);
	}
}

/**
 * Test all other permutations (without iterators)
 */
Test(QuadraticSeeds, permutations)
{
	std::vector<E> permutation (entries.begin(), entries.end());

	auto byId = [](const E& a, const E& b) {
			return a.id < b.id;
		};

	while (std::next_permutation(permutation.begin(), permutation.end(), byId)) {
		QuadraticSeeds<It> seeds (entries.begin(), entries.end());

		cr_assert_eq(seeds.first->id, 1);
		cr_assert_eq(seeds.second->id, 4);
	}
}
