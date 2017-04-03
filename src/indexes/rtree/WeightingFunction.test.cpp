#include <criterion/criterion.h>
#include "WeightingFunction.hpp"
#include "Node.hpp"
#include "Entry.hpp"
#include "CapturingEntryPlugin.hpp"


using namespace Rtree;

constexpr unsigned m = 15;
constexpr unsigned D = 3;
constexpr unsigned M = 100;

template<class WF>
void checkAll(const std::array<double, 11>& correct, unsigned d, WF& wf)
{
	wf.setDimension(d);
	for (unsigned i = 0; i < correct.size(); ++i) {
		double result = wf(10 * i);

		cr_expect_float_eq(
				result, correct[i], 0.04,
				"Expected %.2f to be %.2f (dimension %u, index %u).",
				result, correct[i], d, i
			);
	}
}

/**
 * Test the weighting function using a test case derived from the graph in the
 * paper.
 */
Test(WeightingFunction, paper_test_case)
{
	using N = Node<D, M, CapturingEntryPlugin>;
	using E = typename N::E;

	E parent (
			new N(),
			{E(DataObject{1, Box(Point(3, 0.0), Point(3, 0.0))})}
		);
	E child;

	parent.getMbr() = Box(Point {-0.5, -0.25, 0.0}, Point {0.5, 0.75, 1.0});

	WeightingFunction<E, m> wf (parent);

	// asym = 0
	checkAll(
		{{0.0, 0.07, 0.22, 0.51, 0.85, 1.0, 0.85, 0.51, 0.22, 0.07, 0.0}},
		0,
		wf
	);

	// asym = 0.5
	checkAll(
		{{0.0, 0.04, 0.12, 0.28, 0.51, 0.76, 0.95, 0.99, 0.87, 0.64, 0.38}},
		1,
		wf
	);

	// asym = 1.0
	checkAll(
		{{0.0, 0.03, 0.08, 0.18, 0.31, 0.5, 0.7, 0.88, 0.98, 0.98, 0.88}},
		2,
		wf
	);

	delete parent.getNode();
}
