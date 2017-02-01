#include <criterion/criterion.h>
#include "Point.cpp"
#include "Box.cpp"


Test(axisAlignedBox, contains)
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
