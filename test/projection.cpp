#include "stdafx.h"
#include <unittest.h>
#include <neuralnet.h>

void test_projection()
{
	scenario sc("Projection Test");

	typedef algebra::vector<D10> _Vector;
	typedef machine_learning::projection_2d<D2, D5, _Vector> _Projection;

	_Vector v = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };

	{
		test::verbose("projection::get: arguments in range");
		test::assert(_Projection::get(v, 0, 0, -1.0) == 1.0, "Test failed: get(0,0)");
		test::assert(_Projection::get(v, 1, 2, -1.0) == 6.0, "Test failed: get(1,2)");
		test::assert(_Projection::get(v, 1, 4, -1.0) == 10.0, "Test failed: get(1,4)");
	}

	{
		test::verbose("projection::get: arguments out of range");
		test::assert(_Projection::get(v, 2, 0, -1.0) == -1.0, "Test failed: get(2,0)");
		test::assert(_Projection::get(v, 0, 5, -1.0) == -1.0, "Test failed: get(0,5)");
	}

	{
		_Vector dst;

		test::verbose("projection::set: arguments in range");
		_Projection::set(dst, 1, 2, 15.0);
		test::assert(dst(5) == 15.0, "Test failed: set(1,2)");

		test::verbose("projection::set: arguments out of range");
		_Projection::set(dst, 2, 0, 15.0);
		test::assert(dst(2) == 0.0, "Test failed: set(2,0)");

		_Projection::set(dst, 0, 5, 15.0);
	}

	sc.pass();
}
