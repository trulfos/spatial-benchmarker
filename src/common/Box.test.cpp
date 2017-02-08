#include <criterion/criterion.h>
#include "Point.hpp"
#include "Box.hpp"

Test(Box, contains)
{
	Point a = {2.0f, 4.0f};
	Point b = {100.0f, 200.0f};

	Box box (a, b);

	cr_expect(
			box.contains(Point({5.0f, 5.0f})),
			"AAB between (2, 4) and (100, 200) should contain (5, 5)"
		);

	cr_expect(
			!box.contains({0.0f, 0.0f}),
			"AAB between (2, 4) and (100, 200) should not contain (0, 0)"
		);

	cr_expect(
			box.contains(a),
			"AAB between (2, 4) and (100, 200) should contain the first point"
		);

	cr_expect(
			box.contains(b),
			"AAB between (2, 4) and (100, 200) should contain the second point"
		);

	cr_expect(
			box.contains({2.0f, 5.0f}),
			"AAB between (2, 4) and (100, 200) should contain (2, 4)"
		);
}

Test(Box, intersects)
{
	Box boxA ({Point({1.0f, 2.0f}), Point({3.0f, 4.0f})});
	Box boxB ({Point({2.0f, 1.0f}), Point({4.0f, 3.0f})});
	Box boxC ({Point({10.0f, 10.0f}), Point({11.0f, 11.0f})});
	Box boxD ({Point({3.0f, 4.0f}), Point({5.0f, 5.0f})});


	cr_assert(
			boxA.intersects(boxB),
			"Intersecting boxes reported not to intersect"
		);

	cr_assert(
			boxA.intersects(boxB) == boxB.intersects(boxA) &&
			boxC.intersects(boxA) == boxA.intersects(boxC),
			"Intersection test should work both ways"
		);


	cr_assert_not(
			boxA.intersects(boxC),
			"Not intersecting boxes reported to intersect"
		);

	cr_assert(
			boxA.intersects(boxD),
			"Cornercase intersect not detected"
		);

	cr_assert(
			boxA.intersects(boxA),
			"Box should intersect with itself"
		);
}
