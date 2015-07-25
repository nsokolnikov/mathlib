#include "stdafx.h"
#include <unittest.h>
#include <vector.h>

void test_vector_expressions()
{
	auto v1 = algebra::expressions::declare<algebra::vector<D3>>();
	auto v2 = algebra::expressions::declare<algebra::vector<D3>>();

	v1.set(algebra::vector < D3 > { 1, 2, 3});
	v2.set(algebra::vector < D3 > { -3, -7, -9});

	auto e = v1 + v2;
	auto r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { -2, -5, -6 }, "Test Failed: v1 + v2");

	e = 2 * v1;
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { 2, 4, 6 }, "Test Failed: 2 * v1");

	e = v1 * 2;
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { 2, 4, 6 }, "Test Failed: v1 * 2");

	e = v1 - v2;
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { 4, 9, 12 }, "Test Failed: v1 - v2");

	e = 2 * (v1 + v2);
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { -4, -10, -12 }, "Test Failed: 2 * (v1 + v2)");

	e = (v1 + v2) * 3;
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { -6, -15, -18 }, "Test Failed: (v1 + v2) * 3");

	e = (v1 + v2) + v1;
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { -1, -3, -3 }, "Test Failed: (v1 + v2) + v1");

	e = v2 + (v1 + v2);
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { -5, -12, -15 }, "Test Failed: v2 + (v1 + v2)");

	e = (v1 + v2) + (v1 + v2);
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { -4, -10, -12 }, "Test Failed: (v1 + v2) + (v1 + v2)");

	e = (v1 + v2) - (v1 + v2);
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { 0, 0, 0 }, "Test Failed: (v1 + v2) - (v1 + v2)");

	e = (v1 + v2) - v1;
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { -3, -7, -9 }, "Test Failed: (v1 + v2) - v1");

	e = v1 - (v1 + v2);
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { 3, 7, 9 }, "Test Failed: v1 - (v1 + v2)");

	auto dv1 = algebra::expressions::declare<double>();
	dv1.set(-2);

	e = v1 * dv1;
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { -2, -4, -6 }, "Test Failed: v1 * dv1");

	e = dv1 * v2;
	r = e.evaluate();
	test_assert(r == algebra::vector < D3 > { 6, 14, 18 }, "Test Failed: dv1 * v2");

	test_assert(14 == (v1 * v1).evaluate(), "Test Failed: v1 * v1");

	test_assert(1320 == ((v2 * v1) * ((v1 + v2) * v1)).evaluate(), "Test failed: (v2 * v1) * ((v1 + v2) * v1)");
}

void test_vector()
{
	algebra::vector<D2> v{ 1, 2 };

	test_assert(v == algebra::vector<D2> { 1, 2 }, "Test Failed: v == (1, 2)");
	test_assert(v != algebra::vector<D2> { 2, 2 }, "Test Failed: v != (2, 2)");
	test_assert((2 * v) == algebra::vector<D2> { 2, 4 }, "Test Failed: (2 * v) == (2, 4)");

	auto v1 = algebra::expressions::declare<algebra::vector<D2>>();
	auto v2 = algebra::expressions::declare<algebra::vector<D2>>();

	v1.set(algebra::vector<D2> { 1, 2 });
	v2.set(algebra::vector<D2> { -2, -5 });

	auto e = (2 * v1) * (-1 * v2);
	test_assert(24 == e.evaluate(), "Test Failed: (2 * v1) * (-1 * v2)");

	e = v1 * (-1 * v2);
	test_assert(12 == e.evaluate(), "Test Failed: v1 * (-1 * v2)");

	test_assert(((v1 * (-1 * v2)) * (v1 + v2)).evaluate() == algebra::vector < D2 > { -12, -36 }, "Test Failed: (v1 * (-1 * v2)) * (v1 + v2)");
	test_assert(((-1 * v2) * (v1 * v2)).evaluate() == algebra::vector < D2 > { -24, -60 }, "Test Failed: (-1 * v2) * (v1 * v2)");

	test_assert((-1 * (v1 * v2) * v2).evaluate() == algebra::vector < D2 > { -24, -60 }, "Test Failed: -1 * (v1 * v2) * v2");
	test_assert((v1 * (v2 * v2)).evaluate() == algebra::vector < D2 > { 29, 58 }, "Test Failed: v1 * (v2 * v2)");
}
