#include <criterion/criterion.h>
#include "CheckComp.hpp"
#include "common/DataObject.hpp"
#include "Entry.hpp"

using namespace Rtree;

class Node {};
using E = Entry<2, Node>;


/**
 * This is the example given in the paper.
 */
Test(CheckComp, example_from_paper)
{

	std::array<E, 5> entries = {{
			DataObject(1, {Point {106.0, 41.0}, Point {335.0, 239.0}}),
			DataObject(2, {Point {57.0, 7.0}, Point {298.0, 207.0}}),
			DataObject(3, {Point {166.0, 231.0}, Point {241.0, 321.0}}),
			DataObject(4, {Point {345.0, 51.0}, Point {435.0, 111.0}}),
			DataObject(5, {Point {495.0, 248.0}, Point {568.0, 302.0}})
		}};

	E newEntry (
			DataObject(6, {Point {308.0, 217.0}, Point {381.0, 260.0}})
		);

	using It = typename decltype(entries)::iterator;

	CheckComp<It> checkComp(
			entries.begin(),
			entries.end(),
			newEntry
		);

	// We should be getting the end (no overlap free alternative was found)
	cr_assert_eq(
			checkComp(entries.begin()),
			entries.end()
		);

	cr_expect_eq(checkComp.minOverlap().getId(), 3);
	cr_expect_eq(checkComp.getP() - entries.begin(), 4);

	cr_expect_eq(
			checkComp.getVisited(),
			std::set<It>({
				entries.begin(),
			   	entries.begin() + 2,
				entries.begin() + 3
			})
		);
}
