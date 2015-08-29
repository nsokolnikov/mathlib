#include "stdafx.h"
#include <unittest.h>
#include <matrix.h>
#include <sstream>
void test_matrices() {

	scenario sc("Matrices Test");
/*
	Functions to test:
	transpose
	sum
	subtract
	multiply
	random
	resize
	+
	-
	*
	/
	*/


	auto m1 = algebra::matrix<D3, D3>::random(0, 10);

	auto m2 = algebra::matrix<D3, D3>();

	algebra::matrix<D3, D3> m3{ 
		10, 20, 30, 
		40, 50, 60,
		70, 80, 90 };

	algebra::matrix<D3, D3> m4{
		10, 20, 30,
		40, 50, 60,
		70, 80, 90 };

	algebra::matrix<D3, D3> m5{
		10, 40, 70,
		20, 50, 80,
		30, 60, 90
	};

	algebra::matrix<D3, D3> m6{
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	};

	algebra::matrix<D3, D3> m7{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};

	algebra::matrix<D3, D3> m8{
		1, 1, 1,
		1, 1, 1,
		1, 1, 1
	};

	algebra::matrix<D3, D4> m9{
		1, 1, 1, 0,
		1, 1, 1, 0,
		1, 1, 1, 0,

	};

	algebra::matrix<D3, D3> m5_2{
		20, 80, 140,
		40, 100, 160,
		60, 120, 180
	};

	algebra::matrix<D3, D3> m4_elempow2{
		100,         400,         900,
		1600,        2500,        3600,
		4900,        6400,       8100
	};

	algebra::matrix<D3, D3> m5_m8{
		120,120,120,
		150,150,150,
		180,180,180
	};

	algebra::matrix<D3, D3> m4_pow{
		3000,        3600,        4200,
		6600,        8100,        9600,
		10200,       12600,       15000
	};

	algebra::matrix<D3, D3> m4_abs{
		10, -20, 30,
		-40, 50, -60,
		70, -80, -90 };

	algebra::matrix<D3, D4> eye{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0
	};

	algebra::matrix<D3, D3> m_twos{
		2,2,2,
		2,2,2,
		2,2,2
	};
	
	test::assert(m3 == m3, "Test failed: equality of the same matrix");

	test::assert(m3(0, 0) == 10, "Test Failed: matrix random access");
	test::assert(m3 == m4, "Test failed: equality by value");

	test::assert(m3 == m5.transpose(), "Test failed: Transpose");
	test::assert(m3 - m3 == m6, "Test failed: Matrix subtraction");
	test::assert(m3 + m6 == m3, "Test failed: Matrix addition");
	
	test::assert(m7 * m5 == m5, "Test failed: Matrix multiplication");

	test::assert(m4 * 1 == m3, "Test failed: Multiply matrix by value");

	auto m_mult = m5 * m8;
	test::assert(m_mult != m5 && m_mult != m8, "Test failed: non-destructive multiplication(matrix * matrix)");
	test::assert(m_mult == m5_m8, "Test failed: matrix * matrix");

	auto m2_mult = m5 * 2;
	test::assert(m2_mult == m5_2, "Test failed: matrix * value_type");
	test::assert(m5 != m2_mult, "Test failed: non-destructive matrix * value_type");

	auto m4_copy(m4);
	test::assert(m4_copy.element_pow(2) == m4_elempow2, "Test failed: element-wise power");
	test::assert((m4 ^ 2) == m4_pow, "Test failed: raise matrix to a power");

	test::assert(m4_abs.abs() == m4, "Test failed: absolute value");

	test::assert(algebra::matrix<D3, D4>::eye() == eye, "Test failed: eye()");
	test::assert(algebra::matrix<D3, D3>::eye() == m7, "Test failed: eye()");

	test::assert(m3.max() == 90, "Test failed: max()");
	test::assert(m3.min() == 10, "Test failed: min()");

	test::assert(m3.accumulate() == 450, "Test failed: accumulate()");
	test::assert(algebra::matrix<D3, D3>::ones() == m8, "Test failed: ones");

	auto m10 = m8.resize<D3, D4>();

	test::assert(m10 == m9, "Test failed: resize");

	sc.pass();
}
