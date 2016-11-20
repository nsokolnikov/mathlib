#include "stdafx.h"
#include <unittest.h>
#include <vector.h>

void test_vector_expressions()
{
	scenario sc("Vector Expressions Test");

	{
		test::verbose("D2 vector expressions tests");

		auto v1 = algebra::expressions::declare<algebra::vector<D2>>();
		auto v2 = algebra::expressions::declare<algebra::vector<D2>>();

		v1.set(algebra::vector < D2 > { 1, 2 });
		v2.set(algebra::vector < D2 > { -2, -5 });

		auto e = (2 * v1) * (-1 * v2);
		test::assert(24 == e.evaluate(), "Test Failed: (2 * v1) * (-1 * v2)");

		e = v1 * (-1 * v2);
		test::assert(12 == e.evaluate(), "Test Failed: v1 * (-1 * v2)");

		test::assert(((v1 * (-1 * v2)) * (v1 + v2)).evaluate() == algebra::vector < D2 > { -12, -36 }, "Test Failed: (v1 * (-1 * v2)) * (v1 + v2)");
		test::assert(((-1 * v2) * (v1 * v2)).evaluate() == algebra::vector < D2 > { -24, -60 }, "Test Failed: (-1 * v2) * (v1 * v2)");

		test::assert((-1 * (v1 * v2) * v2).evaluate() == algebra::vector < D2 > { -24, -60 }, "Test Failed: -1 * (v1 * v2) * v2");
		test::assert((v1 * (v2 * v2)).evaluate() == algebra::vector < D2 > { 29, 58 }, "Test Failed: v1 * (v2 * v2)");
	}

	test::verbose("D3 vector expressions tests");

	auto v1 = algebra::expressions::declare<algebra::vector<D3>>();
	auto v2 = algebra::expressions::declare<algebra::vector<D3>>();

	v1.set(algebra::vector < D3 > { 1, 2, 3});
	v2.set(algebra::vector < D3 > { -3, -7, -9});

	auto e = v1 + v2;
	auto r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { -2, -5, -6 }, "Test Failed: v1 + v2");

	e = 2 * v1;
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { 2, 4, 6 }, "Test Failed: 2 * v1");

	e = v1 * 2;
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { 2, 4, 6 }, "Test Failed: v1 * 2");

	e = v1 - v2;
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { 4, 9, 12 }, "Test Failed: v1 - v2");

	e = 2 * (v1 + v2);
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { -4, -10, -12 }, "Test Failed: 2 * (v1 + v2)");

	e = (v1 + v2) * 3;
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { -6, -15, -18 }, "Test Failed: (v1 + v2) * 3");

	e = (v1 + v2) + v1;
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { -1, -3, -3 }, "Test Failed: (v1 + v2) + v1");

	e = v2 + (v1 + v2);
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { -5, -12, -15 }, "Test Failed: v2 + (v1 + v2)");

	e = (v1 + v2) + (v1 + v2);
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { -4, -10, -12 }, "Test Failed: (v1 + v2) + (v1 + v2)");

	e = (v1 + v2) - (v1 + v2);
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { 0, 0, 0 }, "Test Failed: (v1 + v2) - (v1 + v2)");

	e = (v1 + v2) - v1;
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { -3, -7, -9 }, "Test Failed: (v1 + v2) - v1");

	e = v1 - (v1 + v2);
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { 3, 7, 9 }, "Test Failed: v1 - (v1 + v2)");

	auto dv1 = algebra::expressions::declare<double>();
	dv1.set(-2);

	e = v1 * dv1;
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { -2, -4, -6 }, "Test Failed: v1 * dv1");

	e = dv1 * v2;
	r = e.evaluate();
	test::assert(r == algebra::vector < D3 > { 6, 14, 18 }, "Test Failed: dv1 * v2");

	test::assert(14 == (v1 * v1).evaluate(), "Test Failed: v1 * v1");

	test::assert(1320 == ((v2 * v1) * ((v1 + v2) * v1)).evaluate(), "Test failed: (v2 * v1) * ((v1 + v2) * v1)");

	sc.pass();
}

void test_vector()
{
	scenario sc("Vector Operations Test");

	test::verbose("Initializer list tests");
	test::check_exception<std::invalid_argument>(
		[]() { algebra::vector<D1> v{ 1, 2 };  },
		"Invalid initializer list test");

	algebra::vector<D2> v{ 1, 2 };

	test::verbose("Accessors tests");
	test::check_exception<std::invalid_argument>(
		[&v]() { v(10);  },
		"Invalid index test");
	test::check_exception<std::invalid_argument>(
		[&v]() { const algebra::vector<D2> t = v; t(10); },
		"Invalid index test (const)");

	const algebra::vector<D2> t;
	test::assert(0.0 == t(0), "Test Failed: const t(0) == 0.0");

	test::assert(t.empty(), "Test Failed: t.empty()");
	test::assert(false == v.empty(), "Test Failed: !v.empty()");

	test::verbose("Equality operators tests");
	test::assert(v == algebra::vector<D2> { 1, 2 }, "Test Failed: v == (1, 2)");
	test::assert(v != algebra::vector<D2> { 2, 2 }, "Test Failed: v != (2, 2)");

	test::verbose("Assignment operators tests");
	algebra::vector<D2> vt{ 3, 4 };
	vt = vt;
	test::assert(vt == vt, "Test Failed: vt == vt");
	test::assert(vt == algebra::vector<D2> { 3, 4 }, "Test Failed: vt == (3, 4)");

	vt = v;
	test::assert(vt == v, "Test Failed: vt == v");
	test::assert(vt == algebra::vector<D2> { 1, 2 }, "Test Failed: vt == (1, 2)");

	{
		test::verbose("Arithmetic operators tests");
		algebra::vector<D2> v1{ 10, 20 };
		algebra::vector<D2> v2{ 1, 2 };

		test::assert(v1 + v2 == algebra::vector < D2 > { 11, 22 }, "Test Failed: v1 + v2 == (11, 22)");
		test::assert(v1 - v2 == algebra::vector < D2 > { 9, 18 }, "Test Failed: v1 + v2 == (11, 22)");

		v1 -= v2;
		test::assert(v1 == algebra::vector < D2 > { 9, 18 }, "Test Failed: (v1 -= v2) == (9, 18)");

		v1 += v2;
		test::assert(v1 == algebra::vector < D2 > { 10, 20 }, "Test Failed: (v1 += v2) == (10, 20)");

		algebra::vector<D2> v3;
		test::assert(v1 == (v3 + v1), "Test Failed : v1 == (v3 + v1)");
		test::assert(v1 == (v1 + v3), "Test Failed : v1 == (v3 + v1)");
		test::assert(v3 == (v3 + v3), "Test Failed : v3 == (v3 + v3)");
	}

	sc.pass();
}
