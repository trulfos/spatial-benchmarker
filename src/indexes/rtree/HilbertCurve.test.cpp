#include <criterion/criterion.h>
#include <array>
#include <cstdint>
#include "HilbertCurve.hpp"

using Rtree::HilbertCurve;

/**
 * Test case with input and output.
 */
template<unsigned D, unsigned S>
struct TestCase
{
	static constexpr unsigned bits = S;
	static constexpr unsigned dimension = D;
	Point point;
	std::uint64_t answer;
};

/**
 * Checks all test cases.
 */
template<class A>
void checkAll(A cases)
{
	constexpr unsigned dimension = A::value_type::dimension;
	using HC = HilbertCurve<std::uint64_t, A::value_type::dimension>;

	// Create bounding box
	Box bounds (
			Point(dimension, 0.0),
			Point(dimension, 1.0)
		);

	for (auto& c : cases) {
		std::uint64_t r = HC::map(c.point, bounds) >> (8 * sizeof(r) - c.bits);

		cr_expect(
				r == c.answer,
				"Expected %lx to be %lx",
				r, c.answer
			);
	}
}


Test(HilbertCurve, 2d)
{

	// Pretend we have a 4x4 grid
	std::array<TestCase<2, 4>, 16> testCases4 = {{
			{{0.0000f, 0.0000f}, 0},
			{{0.3333f, 0.0000f}, 1},
			{{0.3333f, 0.3333f}, 2},
			{{0.0000f, 0.3333f}, 3},
			{{0.0000f, 0.6666f}, 4},
			{{0.0000f, 1.0000f}, 5},
			{{0.3333f, 1.0000f}, 6},
			{{0.3333f, 0.6666f}, 7},
			{{0.6666f, 0.6666f}, 8},
			{{0.6666f, 1.0000f}, 9},
			{{1.0000f, 1.0000f}, 10},
			{{1.0000f, 0.6666f}, 11},
			{{1.0000f, 0.3333f}, 12},
			{{0.6666f, 0.3333f}, 13},
			{{0.6666f, 0.0000f}, 14},
			{{1.0000f, 0.0000f}, 15},
		}};

	checkAll(testCases4);


	// Some selected cases from a 16x16 grid
	std::array<TestCase<2, 8>, 4> testCases16 = {{
			{{0.0000f, 0.0000f}, 0},
			{{1.0f/15.0f, 11.0f/15.0f}, 76},
			{{7.0f/15.0f, 10.0f/15.0f}, 113},
			{{1.0000f, 0.0000f}, 16*16 - 1},
		}};

	checkAll(testCases16);
}


Test(HilbertCurve, 5d)
{
	std::array<TestCase<5, 20>, 2> testCases = {{
			{ // From paper
				{
					10.0f/15.0f,
					11.0f/15.0f,
					3.00f/15.0f,
					13.0f/15.0f,
					5.00f/15.0f
				},
				0x988b8
			},
			{ // Home made
				{
					10.0f/15.0f,
					15.0f/15.0f,
					2.00f/15.0f,
					5.00f/15.0f,
					14.0f/15.0f
				},
				0x8dcd2
			}
		}};

	checkAll(testCases);
}


Test(HilbertCurve, 10d)
{
	std::array<TestCase<10, 10*4>, 1> testCases = {{
			{
				{
					10.0f/15.0f,
					14.0f/15.0f,
					14.0f/15.0f,
					5.00f/15.0f,
					9.00f/15.0f,
					5.00f/15.0f,
					2.00f/15.0f,
					4.00f/15.0f,
					11.0f/15.0f,
					12.0f/15.0f
				},
				0xb091691e08
			}
	}};

	checkAll(testCases);
}
