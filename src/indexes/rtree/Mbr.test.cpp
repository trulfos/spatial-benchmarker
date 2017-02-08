#include <criterion/criterion.h>
#include "Mbr.hpp"
#include "common/Box.hpp"
#include "common/Point.hpp"

using Rtree::Mbr;

const float EPSILON = 0.001f;

Test(mbr, point_point_distance)
{
	Mbr<2> mbrA (Point({0.0f, 0.0f}));
	Mbr<2> mbrB (Point({0.0f, 1.0f}));

	cr_expect_float_eq(
			mbrA.distance2(mbrB),
			1.0f,
			EPSILON,
			"Distance between points (0,0) and (0,1) incorrect"
		);

	cr_expect_float_eq(
			mbrB.distance2(mbrA),
			1.0f,
			EPSILON,
			"Distance between points (0,1) and (0,0) incorrect"
		);

	cr_expect_float_eq(
			mbrA.distance2(mbrA),
			0.0f,
			EPSILON,
			"Distance between points (0,0) and (0,0) incorrect"
		);
}

Test(mbr, point_box_distance)
{
	Mbr<2> mbrA (Box(
				Point({1.0f, -1.0f}), Point({2.0f, 1.0f})
			));
	Mbr<2> mbrB (Point({0.0f, 0.0f}));
	Mbr<2> mbrC (Point({1.5f, 0.0f}));

	cr_assert_float_eq(
			mbrA.distance2(mbrB),
			1.0f,
			EPSILON,
			"Distance between point and box incorrect"
		);

	cr_assert_float_eq(
			mbrA.distance2(mbrC),
			0.0f,
			EPSILON,
			"Distance between point and box incorrect"
		);
}

Test(mbr, box_box_distance)
{
	Mbr<2> mbrA (Box(
				Point({-1.0f, 0.0f}), Point({0.0f, 1.0f})
			));

	Mbr<2> mbrB (Box(
				Point({1.0f, -3.0f}), Point({2.0f, -2.0f})
			));

	Mbr<2> mbrC (Box(
				Point({-0.5f, 0.5f}), Point({5.0f, 0.8f})
			));

	cr_assert_float_eq(
			mbrA.distance2(mbrB),
			5.0f,
			EPSILON,
			"Distance between box and box incorrect"
		);

	cr_assert_float_eq(
			mbrA.distance2(mbrC),
			0.0f,
			EPSILON,
			"Distance between overlapping boxes should be 0"
		);

	cr_assert_float_eq(
			mbrB.distance2(mbrC),
			2.5f * 2.5f,
			EPSILON,
			"Distance between parallel boxes not correct"
		);
}

Test(mbr, intersects)
{
	Mbr<2> mbrA ({Point({1.0f, 2.0f}), Point({3.0f, 4.0f})});
	Mbr<2> mbrB ({Point({2.0f, 1.0f}), Point({4.0f, 3.0f})});
	Mbr<2> mbrC ({Point({10.0f, 10.0f}), Point({11.0f, 11.0f})});
	Mbr<2> mbrD ({Point({3.0f, 4.0f}), Point({5.0f, 5.0f})});


	cr_assert(
			mbrA.intersects(mbrB),
			"Intersecting MBRs reported not to intersect"
		);

	cr_assert(
			mbrA.intersects(mbrB) == mbrB.intersects(mbrA) &&
			mbrC.intersects(mbrA) == mbrA.intersects(mbrC),
			"Intersection test should work both ways"
		);


	cr_assert_not(
			mbrA.intersects(mbrC),
			"Not intersecting MBRs reported to intersect"
		);

	cr_assert(
			mbrA.intersects(mbrD),
			"Cornercase intersect not detected"
		);

	cr_assert(
			mbrA.intersects(mbrA),
			"MBR should intersect with itself"
		);
}

Test(mbr, intersection)
{
	Mbr<2> mbrA ({Point({1.0f, 2.0f}), Point({3.0f, 4.0f})});
	Mbr<2> mbrB ({Point({2.0f, 1.0f}), Point({4.0f, 3.0f})});

	cr_assert_float_eq(
			mbrA.intersection(mbrB).volume(),
			1.0f,
			EPSILON,
			"Volume of intersection incorrect"
		);

	cr_assert_float_eq(
			mbrA.intersection(mbrA).volume(),
			mbrA.volume(),
			EPSILON,
			"Intersection with itself should give itself"
		);
}

Test(mbr, perimeter)
{
	cr_assert_float_eq(
			Mbr<2>({Point({1.0f, 2.0f}), Point({3.0f, 4.0f})}).perimeter(),
			8.0f,
			EPSILON,
			"Perimeter of 2x2 MBR should be 8"
		);

	cr_assert_float_eq(
			Mbr<2>({Point({0.0f, 0.0f}), Point({1.0f, 0.0f})}).perimeter(),
			2.0f,
			EPSILON,
			"Perimeter of 1x0 MBR should be 2"
		);

	cr_assert_float_eq(
			Mbr<2>({Point({0.0f, 0.0f}), Point({0.0f, 0.0f})}).perimeter(),
			0.0f,
			EPSILON,
			"Perimeter of 0x0 MBR should be 0"
		);
}

Test(mbr, center)
{
	cr_assert_float_eq(
			Mbr<2>({Point({1.0f, 2.0f}), Point({3.0f, 4.0f})}).center()[0],
			2.0f,
			EPSILON,
			"MBR should have center at x=2"
		);

	cr_assert_float_eq(
			Mbr<2>({Point({1.0f, 2.0f}), Point({3.0f, 4.0f})}).center()[1],
			3.0f,
			EPSILON,
			"MBR should have center at y=3"
		);
}
