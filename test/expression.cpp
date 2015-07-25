#include "stdafx.h"
#include "unittest.h"
#include "../src/expression.h"

void test_expressions()
{
	auto v1 = algebra::expressions::declare<int>();
	auto v2 = algebra::expressions::declare<int>();

	v1.set(10);
	v2.set(20);

	auto e = v1 + v2;
	auto r = e.evaluate();
	test_assert(r == 30, "Test Failed: v1 + v2");

	e = 2 * v1;
	r = e.evaluate();
	test_assert(r == 20, "Test Failed: 2 * v1");

	e = v1 * 3;
	r = e.evaluate();
	test_assert(r == 30, "Test Failed: v1 * 3");

	e = v1 - v2;
	r = e.evaluate();
	test_assert(r == -10, "Test Failed: v1 - v2");

	e = 2 * (v1 + v2);
	r = e.evaluate();
	test_assert(r == 60, "Test Failed: 2 * (v1 + v2)");

	e = (v1 + v2) * 3;
	r = e.evaluate();
	test_assert(r == 90, "Test Failed: (v1 + v2) * 3");

	e = (v1 + v2) + v1;
	r = e.evaluate();
	test_assert(r == 40, "Test Failed: (v1 + v2) + v1");

	e = v2 + (v1 + v2);
	r = e.evaluate();
	test_assert(r == 50, "Test Failed: v2 + (v1 + v2)");

	e = (v1 + v2) + (v1 + v2);
	r = e.evaluate();
	test_assert(r == 60, "Test Failed: (v1 + v2) + (v1 + v2)");

	e = (v1 + v2) - (v1 + v2);
	r = e.evaluate();
	test_assert(r == 0, "Test Failed: (v1 + v2) - (v1 + v2)");

	e = (v1 + v2) - v1;
	r = e.evaluate();
	test_assert(r == 20, "Test Failed: (v1 + v2) - v1");

	e = v1 - (v1 + v2);
	r = e.evaluate();
	test_assert(r == -20, "Test Failed: v1 - (v1 + v2)");
}