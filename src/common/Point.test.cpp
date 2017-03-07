#include <criterion/criterion.h>
#include "Point.hpp"


Test(Point, difference)
{
	Point a = {2.0f, 4.0f};
	Point b = {100.0f, 200.0f};

	//TODO: Float comparison - should be within a delta

	cr_expect_eq(
			b - a,
			Point({98.0f, 196.0f}),
			"Difference between (100, 200) and (2, 4) should be (98, 196)"
		);

	cr_expect_eq(
			b - Point({0.0f, 0.0f}),
			b,
			"A point minus (0,0) should be the point itself"
		);
}


Test(Point, multiplication)
{
	Point a = {2.0f, 4.0f};
	Point b = {100.0f, 200.0f};

	cr_expect_float_eq(
			b * a,
			1000.0f,
			0.01f,
			"Multiplying (2, 4) by (100, 200) should yield 1000"
		);

	cr_expect_float_eq(
			a * Point({0.0f, 0.0f}),
			0.0f,
			0.01f,
			"Multiplying (2, 4) by (0, 0) should yield 0"
		);
}


Test(Point, squared)
{
	Point a {2.0f, 5.0f};

	cr_expect_float_eq(
			a.squared(),
			29.0f,
			0.01f,
			"Squaring the point (2, 5) should give 29"
		);
}
