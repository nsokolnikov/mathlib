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

void test_matrix_row_iterators()
{
	scenario sc("Matrix Row Iterators Test");

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
void test_matrix_column_iterators()
{
	scenario sc("Matrix Column Iterators Test");

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
			[&m]() { algebra::const_column_iterator<_Matrix> it(m, 5);  },
			"Invalid column");

		test::check_exception<std::invalid_argument>(
			[&m]() { algebra::column_iterator<_Matrix> it(m, 5);  },
			"Invalid row");
	}

	{
		test::verbose("Constructor tests: positive");

		algebra::const_column_iterator<_Matrix> cit1(m, 0);
		algebra::const_column_iterator<_Matrix> cit2(m, 1);
		algebra::const_column_iterator<_Matrix> cit3(m, 2);
		algebra::const_column_iterator<_Matrix> cit4(m, 3);

		algebra::column_iterator<_Matrix> it1(m, 0);
		algebra::column_iterator<_Matrix> it2(m, 1);
		algebra::column_iterator<_Matrix> it3(m, 2);
		algebra::column_iterator<_Matrix> it4(m, 3);

		auto ccopy = cit2;
		auto copy = it2;
	}
	test::verbose("Constant iterators");
	test_iterator_operators(m.column_begin(0), m.column_end(0), _Matrix::row_rank);

	test::verbose("Writable iterators");
	test_iterator_operators(m.row_begin(0), m.row_end(0), _Matrix::column_rank);
	test::verbose("Dereference Operators");
	for (size_t row = 0; row < _Matrix::row_rank; ++row)
	{
		for (size_t col = 0; col < _Matrix::column_rank; ++col)
		{
			std::stringstream test;
			test << " for row=" << row << "; col=" << col;

			test::assert(m(row, col) == *(m.column_begin(col) + row), ("Test Failed: m(row, col) == *(m.row_begin(row) + col)" + test.str()).c_str());
			test::assert(m(row, col) == *(m.ccolumn_begin(col) + row), ("Test Failed: m(row, col) == *(m.row_begin(row) + col)" + test.str()).c_str());

			double value = row * 100.0 + col;
			test::assert(value == (*(m.column_begin(col) + row) = value), ("Test Failed: value == (*(m.row_begin(row) + col) = value)" + test.str()).c_str());
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

void test_matrix_view_row_iterators()
{
	scenario sc("View Row Iterators Test");

	typedef algebra::matrix<D4, D5> _Matrix;
	typedef algebra::const_view<D3, D3, const _Matrix> _ConstView;
	typedef algebra::view<D3, D3, _Matrix> _View;

	_Matrix m = {
		11, 12, 13, 14, 15,
		21, 22, 23, 24, 25,
		31, 32, 33, 34, 35,
		41, 42, 43, 44, 45
	};

	_ConstView cv = m.make_const_view<D3, D3>(1, 2);
	_View v = m.make_view<D3, D3>(0, 1);

	{
		test::verbose("Constructor tests: const view: negative");
		test::check_exception<std::invalid_argument>(
			[&cv]() { algebra::const_row_iterator<_ConstView> it(cv, 4);  },
			"Invalid row");
		test::check_exception<std::invalid_argument>(
			[&cv]() { algebra::row_iterator<_ConstView> it(cv, 4);  },
			"Invalid row");
	}

	{
		test::verbose("Constructor tests: view: negative");
		test::check_exception<std::invalid_argument>(
			[&v]() { algebra::const_row_iterator<_View> it(v, 4);  },
			"Invalid row");
		test::check_exception<std::invalid_argument>(
			[&v]() { algebra::row_iterator<_View> it(v, 4);  },
			"Invalid row");
	}

	{
		test::verbose("Constructor tests: const view: positive");

		algebra::const_row_iterator<_ConstView> cit1(cv, 0);
		algebra::const_row_iterator<_ConstView> cit2(cv, 1);
		algebra::const_row_iterator<_ConstView> cit3(cv, 2);

		algebra::row_iterator<_ConstView> it1(cv, 0);
		algebra::row_iterator<_ConstView> it2(cv, 1);
		algebra::row_iterator<_ConstView> it3(cv, 2);

		auto ccopy = cit2;
		auto copy = it2;
	}

	{
		test::verbose("Constructor tests: view: positive");

		algebra::const_row_iterator<_View> cit1(v, 0);
		algebra::const_row_iterator<_View> cit2(v, 1);
		algebra::const_row_iterator<_View> cit3(v, 2);

		algebra::row_iterator<_View> it1(v, 0);
		algebra::row_iterator<_View> it2(v, 1);
		algebra::row_iterator<_View> it3(v, 2);

		auto ccopy = cit2;
		auto copy = it2;
	}

	test::verbose("Constant view iterators: const view");
	test_iterator_operators(cv.row_begin(0), cv.row_end(0), _ConstView::column_rank);

	test::verbose("Constant view iterators: view");
	test_iterator_operators(v.row_begin(0), v.row_end(0), _View::column_rank);

	test::verbose("Writable iterators: view");
	test_iterator_operators(v.row_begin(0), v.row_end(0), _View::column_rank);

	test::verbose("Dereference Operators: const view");
	for (size_t row = 0; row < _ConstView::row_rank; ++row)
	{
		for (size_t col = 0; col < _ConstView::column_rank; ++col)
		{
			std::stringstream test;
			test << " for row=" << row << "; col=" << col;
			test::assert(m(row + 1, col + 2) == *(cv.row_begin(row) + col), ("Test Failed: m(row + 1, col + 2) == *(cv.row_begin(row) + col)" + test.str()).c_str());
		}

		test::check_exception<std::invalid_argument>(
			[&cv, row]() { *(cv.row_end(row));  },
			"Reading past row end: const iterator");
	}

	test::verbose("Dereference Operators: view");
	for (size_t row = 0; row < _View::row_rank; ++row)
	{
		for (size_t col = 0; col < _View::column_rank; ++col)
		{
			std::stringstream test;
			test << " for row=" << row << "; col=" << col;
			test::assert(m(row, col + 1) == *(v.row_begin(row) + col), ("Test Failed: m(row, col + 1) == *(v.row_begin(row) + col))" + test.str()).c_str());
			test::assert(m(row, col + 1) == *(v.crow_begin(row) + col), ("Test Failed: m(row, col + 1) == *(v.crow_begin(row) + col)" + test.str()).c_str());

			double value = row * 100.0 + col;
			test::assert(value == (*(v.row_begin(row) + col) = value), ("Test Failed: value == (*(v.row_begin(row) + col) = value)" + test.str()).c_str());
			test::assert(m(row, col + 1) == value, ("Test Failed: m(row, col + 1) == value" + test.str()).c_str());
		}

		test::check_exception<std::invalid_argument>(
			[&v, row]() { *(v.crow_end(row));  },
			"Reading past row end: const iterator");
		test::check_exception<std::invalid_argument>(
			[&v, row]() { *(v.row_end(row));  },
			"Reading past row end: iterator");
		test::check_exception<std::invalid_argument>(
			[&v, row]() { *(v.row_end(row)) = -10.0;  },
			"Writing past row end: iterator");
	}

	sc.pass();
}

void test_matrix_view_column_iterators()
{
	scenario sc("View Column Iterators Test");
	typedef algebra::matrix<D4, D5> _Matrix;
	typedef algebra::const_view<D3, D3, const _Matrix> _ConstView;
	typedef algebra::view<D3, D3, _Matrix> _View;

	_Matrix m = {
		11, 12, 13, 14, 15,
		21, 22, 23, 24, 25,
		31, 32, 33, 34, 35,
		41, 42, 43, 44, 45
	};

	_ConstView cv = m.make_const_view<D3, D3>(1, 2);
	_View v = m.make_view<D3, D3>(0, 1);

	{
		test::verbose("Constructor tests: const view: negative");
		test::check_exception<std::invalid_argument>(
			[&cv]() { algebra::const_column_iterator<_ConstView> it(cv, 5);  },
			"Invalid column");
		test::check_exception<std::invalid_argument>(
			[&cv]() { algebra::column_iterator<_ConstView> it(cv, 5);  },
			"Invalid column");
	}

	{
		test::verbose("Constructor tests: view: negative");
		test::check_exception<std::invalid_argument>(
			[&v]() { algebra::const_column_iterator<_View> it(v, 5);  },
			"Invalid column");
		test::check_exception<std::invalid_argument>(
			[&v]() { algebra::column_iterator<_View> it(v, 5);  },
			"Invalid column");
	}

	{
		test::verbose("Constructor tests: const view: positive");

		algebra::const_column_iterator<_ConstView> cit1(cv, 0);
		algebra::const_column_iterator<_ConstView> cit2(cv, 1);
		algebra::const_column_iterator<_ConstView> cit3(cv, 2);

		algebra::column_iterator<_ConstView> it1(cv, 0);
		algebra::column_iterator<_ConstView> it2(cv, 1);
		algebra::column_iterator<_ConstView> it3(cv, 2);

		auto ccopy = cit2;
		auto copy = it2;
	}

	{
		test::verbose("Constructor tests: view: positive");

		algebra::const_column_iterator<_View> cit1(v, 0);
		algebra::const_column_iterator<_View> cit2(v, 1);
		algebra::const_column_iterator<_View> cit3(v, 2);

		algebra::column_iterator<_View> it1(v, 0);
		algebra::column_iterator<_View> it2(v, 1);
		algebra::column_iterator<_View> it3(v, 2);

		auto ccopy = cit2;
		auto copy = it2;
	}

	test::verbose("Constant view iterators: const view");
	test_iterator_operators(cv.column_begin(0), cv.column_end(0), _ConstView::row_rank);

	test::verbose("Constant view iterators: view");
	test_iterator_operators(v.ccolumn_begin(0), v.ccolumn_end(0), _View::column_rank);

	test::verbose("Writable iterators: view");
	test_iterator_operators(v.column_begin(0), v.column_end(0), _View::column_rank);

	test::verbose("Dereference Operators: const view");
	for (size_t row = 0; row < _ConstView::row_rank; ++row)
	{
		for (size_t col = 0; col < _ConstView::column_rank; ++col)
		{
			
			std::stringstream test;
			test << " for row=" << row << "; col=" << col << ";value = " << m(row + 1, col + 2) << ";itvalue = " << *(cv.column_begin(col) + row);
			test::assert(m(row + 1, col + 2) == *(cv.column_begin(col) + row), ("Test Failed: m(row + 2, col + 1) == *(cv.column_begin(row) + col)" + test.str()).c_str());
			
			test::check_exception<std::invalid_argument>(
				[&cv, row]() { *(cv.row_end(row));  },
				"Reading past row end: const iterator");
		}
	}
	
	test::verbose("Dereference Operators: view");
	for (size_t col = 0; col < _View::column_rank; ++col)
	{
		for (size_t row = 0; row < _View::row_rank; ++row)
		{
			std::stringstream test;
			test << " for row=" << row << "; col=" << col;
			test::assert(m(row, col + 1) == *(v.column_begin(col) + row), ("Test Failed: m(row, col + 1) == *(v.column_begin(col) + row))" + test.str()).c_str());
			test::assert(m(row, col + 1) == *(v.ccolumn_begin(col) + row), ("Test Failed: m(row, col + 1) == *(v.ccolumn_begin(col) + row)" + test.str()).c_str());

			double value = row * 100.0 + col;
			test::assert(value == (*(v.column_begin(col) + row) = value), ("Test Failed: value == (*(v.row_begin(row) + col) = value)" + test.str()).c_str());
			test::assert(m(row, col + 1) == value, ("Test Failed: m(row, col + 1) == value" + test.str()).c_str());
		}

		test::check_exception<std::invalid_argument>(
			[&v, col]() { *(v.ccolumn_end(col));  },
			"Reading past row end: const iterator");
		test::check_exception<std::invalid_argument>(
			[&v, col]() { *(v.column_end(col));  },
			"Reading past row end: iterator");
		test::check_exception<std::invalid_argument>(
			[&v, col]() { *(v.column_end(col)) = -10.0;  },
			"Writing past row end: iterator");
	}
	
	sc.pass();
}

void test_vector_iterators()
{
	scenario sc("Vector Iterators Test");

	typedef algebra::vector<D4> _Vector;

	_Vector v{ 10, 20, 30, 40 };

	{
		test::verbose("Constructor tests: positive");

		algebra::const_vector_iterator<_Vector> cit1(v);

		algebra::const_vector_iterator<_Vector> it1(v);

		auto ccopy = cit1;
		auto copy = it1;
	}

	test::verbose("Constant vector iterators");
	test_iterator_operators(v.cbegin(), v.cend(), _Vector::rank);

	test::verbose("Writable vector iterators");
	test_iterator_operators(v.begin(), v.end(), _Vector::rank);

	test::verbose("Dereference Operators: const vector");
	for (size_t i = 0; i < _Vector::rank; ++i)
	{
		std::stringstream test;
		test << " for i=" << i << "; value = " << v(i) << "; itvalue = " << *(v.begin() + i);
		test::assert(v(i) == *(v.begin() + i), ("Test Failed: v(i) == *(v.begin() + i)" + test.str()).c_str());
	}

	test::check_exception<std::invalid_argument>(
		[&v]() { *(v.end());  },
		"Reading past vector end: const iterator");

	test::verbose("Dereference Operators: vector");
	for (size_t i = 0; i < _Vector::rank; ++i)
	{
		std::stringstream test;
		test << " for i=" << i;
		test::assert(v(i) == *(v.begin() + i), ("Test Failed: v(i) == *(v.begin() + i)" + test.str()).c_str());
		test::assert(v(i) == *(v.cbegin() + i), ("Test Failed: v(i) == *(v.cbegin() + i)" + test.str()).c_str());

		double value = i * 100.0;
		test::assert(value == (*(v.begin() + i) = value), ("Test Failed: (*(v.begin() + i) = value)" + test.str()).c_str());
		test::assert(v(i) == value, ("Test Failed: v(i) == value" + test.str()).c_str());
	}

	test::check_exception<std::invalid_argument>(
		[&v]() { *(v.cend());  },
		"Reading past vector end: const iterator");

	test::check_exception<std::invalid_argument>(
		[&v]() { *(v.end());  },
		"Reading past vector end: iterator");

	sc.pass();
}
