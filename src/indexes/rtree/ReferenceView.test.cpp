#include <criterion/criterion.h>
#include "ReferenceView.hpp"

using namespace Rtree;

std::array<const unsigned, 10> values = {{
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	}};

Test(ReferenceView, iterator_constructor)
{
	ReferenceView<typename decltype(values)::iterator> view (
			values.begin(),
			values.end()
		);

	auto it = view.begin();
	for (unsigned i = 0; i < values.size(); i++) {
		cr_expect_eq(*it, i + 1);
		cr_assert_neq(it, view.end());
		++it;
	}

	cr_expect_eq(it, view.end());
}

Test(ReferenceView, sort_method)
{
	ReferenceView<typename decltype(values)::iterator> view (
			values.begin(),
			values.end()
		);

	view.sort([](const unsigned& a, const unsigned& b) {
			return b < a;
		});


	auto it = view.begin();
	for (unsigned i = 0; i < values.size(); i++) {
		cr_expect_eq(*it, values.size() - i);
		cr_assert_neq(it, view.end());
		++it;
	}

	cr_expect_eq(it, view.end());
}

Test(ReferenceView, copy_constructor)
{
	ReferenceView<typename decltype(values)::iterator> view0 (
			values.begin(),
			values.end()
		);

	ReferenceView<typename decltype(values)::iterator> view (view0);

	// Mess with original view
	view0.sort([](const unsigned& a, const unsigned& b) {
			return b < a;
		});

	auto it = view.begin();
	for (unsigned i = 0; i < values.size(); i++) {
		cr_expect_eq(*it, i + 1);
		cr_assert_neq(it, view.end());
		++it;
	}

	cr_expect_eq(it, view.end());
}
