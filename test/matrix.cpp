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

	algebra::matrix<D3, D3> m3_2(m3);

	algebra::matrix<D3, D3> m4(std::vector<double>{
		10, 20, 30,
		40, 50, 60,
		70, 80, 90 });

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
	
	test::assert(m3 == m3, "Test failed: equality of the same matrix");
	test::assert(m3 == m3_2, "Test failed: copy constructor");

	test::assert(m3(0, 0) == 10, "Test Failed: matrix random access");
	test::assert(m3 == m4, "Test failed: equality by value");

	test::assert(m3 == m5.transpose(), "Test failed: Transpose");
	test::assert(m3 - m3 == m6, "Test failed: Matrix subtraction");
	test::assert(m3 + m6 == m3, "Test failed: Matrix addition");
	
	test::assert(m7 * m5 == m5, "Test failed: Matrix multiplication");

	test::assert(m4 * 1 == m3, "Test failed: Multiply matrix by value");
	
	auto m10 = m8.resize<D3, D4>();

	test::assert(m10 == m9, "Test failed: resize");

	sc.pass();
}

void test_sparse_matrices() {
	scenario sc("Sparse Matrices Test");


	algebra::sparse_matrix<D3, D3> m3{
		10, 20, 30,
		40, 50, 60,
		70, 80, 90 };

	algebra::sparse_matrix<D3, D3> m4(std::vector<double>{
		10, 20, 30,
		40, 50, 60,
		70, 80, 90 });

	algebra::sparse_matrix<D3, D3> m5(std::vector<double>{
		10, 40, 70,
		20, 50, 80,
		30, 60, 90
	});

	algebra::sparse_matrix<D3, D3> m6(std::vector<double>{
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	});

	algebra::sparse_matrix<D3, D3> m7(std::vector<double>{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	});

	algebra::sparse_matrix<D3, D3> m8(std::vector<double>{
		1, 1, 1,
		1, 1, 1,
		1, 1, 1
	});

	algebra::sparse_matrix<D3, D4> m9(std::vector<double>{
		1, 1, 1, 0,
		1, 1, 1, 0,
		1, 1, 1, 0,

	});

	algebra::sparse_matrix<D4, D6> m10{
		10, 20,  0, 30, 0, 10,
		0,  0,   0,  0,  0,  0,
		70, 80, 90, 20, 40, 20,
		0,  0,   0,  0,  0,  1};

	test::assert(m3 == m3, "Test failed: equality of the same matrix");
	test::assert(m3(0, 0) == 10, "Test Failed: matrix random access");
	test::assert(m7(1, 0) == 0, "Test Failed: random access a zero element");
	test::assert(m3 == m4, "Test failed: equality by value");
	test::assert(m3 == m5.transpose(), "Test failed: Transpose");
	m3.to_yale();
	m3.print_yale();
	for (size_t i = 1; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			double yale = m3.yale_get(i, j);
			double regular = m3(i, j);
			test::assert(m3.yale_get(i, j) == m3(i, j), "Test failed: yale random access");
		}
	}
	m10.to_yale();
	for (size_t i = 1; i < 4; i++) {
		for (size_t j = 0; j < 6; j++) {
			double yale = m10.yale_get(i, j);
			double regular = m10(i, j);
			test::assert(m10.yale_get(i, j) == m10(i, j), "Test failed: yale random access");
		}
	}
	sc.pass();

}