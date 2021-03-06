#include <criterion/criterion.h>
#include "GoalFunction.hpp"
#include "SplitSet.hpp"
#include "Entry.hpp"
#include "EntryPlugin.hpp"
#include "Link.hpp"

constexpr float EPSILON = 0.001f;
constexpr unsigned D = 2;

using namespace Rtree;

struct Node {
	using Mbr = ::Rtree::Mbr<D>;
	using Link = ::Rtree::Link<Node>;
	using Plugin = ::Rtree::EntryPlugin;

	using iterator = std::array<Entry<Node>, 3>::iterator;
};

using E = Entry<Node>;
using M = typename E::Mbr;


/**
 * Two equal points in a split.
 */
Test(GoalFunction, volumeless)
{
	Point p (D, 0.0);
	Box b (p, p);
	M mbr (p);

	std::array<E, 2> entries = {{
			DataObject(1, b),
			DataObject(2, b)
		}};

	GoalFunction gf (mbr);

	for (unsigned d = 0; d < 2 * D; d++) {
		Split<Node> split (
				ReferenceView<typename decltype(entries)::iterator>(
						entries.begin(), entries.end()
					),
				D / 2,
				D % 2,
				1
			);

		cr_expect_float_eq(
				gf.getMaxPerimeter(),
				0.0,
				EPSILON
			);

		cr_expect_float_eq(
				gf(split, false),
				0.0,
				EPSILON
			);
	}
}

/**
 * Simple example looking something like this:
 *
 *     +-----+
 *     |     |
 *     |  1  |
 *  +--------------------+
 *  |  |     |           |   +--------+
 *  |  |     |           |   |   3    |
 *  |  |     |           |   +--------+
 *  |  |     |   2       |
 *  |  +-----+           |
 *  |                    |
 *  +--------------------+
 *
 */
Test(GoalFunction, simple)
{
	std::array<E, 3> entries = {{
			DataObject(1, Box(Point {1.0, 3.0}, Point {3.0, 6.0})),
			DataObject(2, Box(Point {0.0, 2.0}, Point {5.0, 5.0})),
			DataObject(3, Box(Point {6.0, 3.5}, Point {8.0, 4.5}))
		}};


	GoalFunction gf (
			std::accumulate(
					entries.begin(), entries.end(),
					entries[0].getMbr(),
					[](const M& sum, const E& entry) {
						return sum + entry.getMbr();
					}
				)
		);

	cr_expect_float_eq(
			gf.getMaxPerimeter(),
			20.0,
			EPSILON
		);

	SplitSet<Node, 1> splits (entries.begin(), entries.end());

	std::array<double, 8> scores = {{
		4.0, -8.0, 8.0, -8.0, -8.0, 4.0, 8.0, -8.0
	}};

	for (const Split<Node>& split : splits) {
		cr_expect_float_eq(
				gf(split, true),
				scores[
						split.getDimension() * 4 +
						split.getSort() * 2 +
						split.getSplitPoint() - 1
					],
				EPSILON
			);
	}
}
