#include "stdafx.h"
#include <unittest.h>
#include <matrix.h>

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

	test::assert(m3(0, 0) == 10, "Test Failed: matrix random access");

//	algebra::matrix<D4, D5> m = {
//		11, 12, 13, 14, 15,
//		21, 22, 23, 24, 25,
//		31, 32, 33, 34, 35,
//		41, 42, 43, 44, 45
//	};

//	m.column_rank;
	
	sc.pass();
}