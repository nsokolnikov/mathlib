#include "stdafx.h"
#include <unittest.h>
#include <matrix.h>
#include <sstream>

template <class _It>
void test_iterator_operators(
	_It it,
	_It end,
	const typename _It::difference_type distance)
{
	auto it2 = it;

	test::verbose("Arithmetic operators tests");

	test::assert(it == it2, "Test Failed: it == it2");
	test::assert(it < (it2 + 1), "Test Failed: it < (it2 + 1)");
	test::assert(it <= (it2 + 1), "Test Failed: it <= (it2 + 1)");
	test::assert(it != (it2 + 1), "Test Failed: it <= (it2 + 1)");

	test::assert((it + 1) > it2, "Test Failed: (it + 1) > it2");
	test::assert((1 + it) >= it2, "Test Failed: (it + 1) >= it2");

	test::assert(it <= end - 2, "Test Failed: it <= end - 2");

	test::assert(0 == it - it2, "Test Failed: 0 == it - it2");
	test::assert(distance == end - it, "Test Failed: distance == end - it");

	test::verbose("Increment operators tests");
	test::assert(it + 1 == (it + 1)++, "Test Failed: it + 1 == (it + 1)++");
	test::assert(it + 2 == ++(it + 1), "Test Failed: it + 2 == ++(it + 1)");
	test::assert(it + 2 == ((it + 1) += 1), "Test Failed: it + 2 == ((it + 1) += 1)");

	test::verbose("Decrement operators tests");
	test::assert(it + 2 == (it + 2)--, "Test Failed: it + 2 == (it + 2)--");
	test::assert(it + 1 == --(it + 2), "Test Failed: it + 1 == --(it + 2)");
	test::assert(it == ((it + 1) -= 1), "Test Failed: it == ((it + 1) -= 1)");

	test::verbose("Positive overflow test");
	test::assert(end == (it + (distance + 1)), "Test Failed: end == (it + (distance + 1))");
	test::assert(it == (it - distance), "Test Failed: it == (it - distance)");
}

void test_row_iterators()
{
	scenario sc("Row Iterators Test");

	typedef algebra::matrix<D4, D5> _Matrix;

	_Matrix m = {
		11, 12, 13, 14, 15,
		21, 22, 23, 24, 25,
		31, 32, 33, 34, 35,
		41, 42, 43, 44, 45
	};

	{
		test::verbose("Constructor tests: negative");
		test::check_exception<std::invalid_argument>(
			[&m]() { algebra::const_row_iterator<_Matrix> it(m, 4);  },
			"Invalid row");
		test::check_exception<std::invalid_argument>(
			[&m]() { algebra::row_iterator<_Matrix> it(m, 4);  },
			"Invalid row");
	}

	{
		test::verbose("Constructor tests: positive");

		algebra::const_row_iterator<_Matrix> cit1(m, 0);
		algebra::const_row_iterator<_Matrix> cit2(m, 1);
		algebra::const_row_iterator<_Matrix> cit3(m, 2);
		algebra::const_row_iterator<_Matrix> cit4(m, 3);

		algebra::row_iterator<_Matrix> it1(m, 0);
		algebra::row_iterator<_Matrix> it2(m, 1);
		algebra::row_iterator<_Matrix> it3(m, 2);
		algebra::row_iterator<_Matrix> it4(m, 3);

		auto ccopy = cit2;
		auto copy = it2;
	}

	test::verbose("Constant iterators");
	test_iterator_operators(m.crow_begin(0), m.crow_end(0), _Matrix::column_rank);

	test::verbose("Writable iterators");
	test_iterator_operators(m.row_begin(0), m.row_end(0), _Matrix::column_rank);

	test::verbose("Dereference Operators");
	for (size_t row = 0; row < _Matrix::row_rank; ++row)
	{
		for (size_t col = 0; col < _Matrix::column_rank; ++col)
		{
			std::stringstream test;
			test << " for row=" << row << "; col=" << col;
			test::assert(m(row, col) == *(m.row_begin(row) + col), ("Test Failed: m(row, col) == *(m.row_begin(row) + col)" + test.str()).c_str());
			test::assert(m(row, col) == *(m.crow_begin(row) + col), ("Test Failed: m(row, col) == *(m.row_begin(row) + col)" + test.str()).c_str());

			double value = row * 100.0 + col;
			test::assert(value == (*(m.row_begin(row) + col) = value), ("Test Failed: value == (*(m.row_begin(row) + col) = value)" + test.str()).c_str());
			test::assert(m(row, col) == value, ("Test Failed: m(row, col) == value" + test.str()).c_str());
		}

		test::check_exception<std::invalid_argument>(
			[&m, row]() { *(m.row_end(row));  },
			"Reading past row end: const iterator");
		test::check_exception<std::invalid_argument>(
			[&m, row]() { *(m.row_end(row));  },
			"Reading past row end: iterator");
	}

	sc.pass();
}
