#include "stdafx.h"
#include <unittest.h>
#include <algorithm.h>

void test_solve()
{
	scenario sc("Test for algebra::solve");

	for (int i = 0; i < 10; i++)
	{
		auto a = algebra::matrix<D4, D4>::random(0, 10);
		algebra::vector<D4> b {10, 20, 30, 40 };
		algebra::vector<D4> x;

		if (algebra::solve(a, b, x))
		{
			test::assert(a * x == b, "Test Failed: solution is incorrect");
		}
	}

	sc.pass();
}