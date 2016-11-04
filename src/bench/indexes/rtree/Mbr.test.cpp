#include <criterion/criterion.h>
#include "Mbr.hpp"
#include "../../common/AxisAlignedBox.cpp"
#include "../../common/Point.cpp"

using namespace Rtree;

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
	Mbr<2> mbrA (AxisAlignedBox(
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
	Mbr<2> mbrA (AxisAlignedBox(
				Point({-1.0f, 0.0f}), Point({0.0f, 1.0f})
			));

	Mbr<2> mbrB (AxisAlignedBox(
				Point({1.0f, -3.0f}), Point({2.0f, -2.0f})
			));

	Mbr<2> mbrC (AxisAlignedBox(
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


//TODO: test 3d
