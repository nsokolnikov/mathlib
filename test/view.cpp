#include "stdafx.h"
#include <unittest.h>
#include <matrix.h>
#include <sstream>

void test_view()
{
	scenario sc("Matrix View Test");

	typedef algebra::matrix<D4, D5> _Matrix;
	typedef algebra::const_view<D2, D2, const _Matrix> _ConstView;
	typedef algebra::view<D2, D2, _Matrix> _View;

	_Matrix m = {
		11, 12, 13, 14, 15,
		21, 22, 23, 24, 25,
		31, 32, 33, 34, 35,
		41, 42, 43, 44, 45
	};

	{
		test::verbose("Constructor tests: const view: negative");
		test::check_exception<std::invalid_argument>(
			[&m]() { _ConstView cv(m, 0, 6);  },
			"Invalid view column");
		test::check_exception<std::invalid_argument>(
			[&m]() { _ConstView cv(m, 0, 4);  },
			"Invalid view column");
		test::check_exception<std::invalid_argument>(
			[&m]() { _ConstView cv(m, 5, 0);  },
			"Invalid view row");
		test::check_exception<std::invalid_argument>(
			[&m]() { _ConstView cv(m, 3, 0);  },
			"Invalid view row");

		test::verbose("Matrix::make_const_view: negative");
		test::check_exception<std::invalid_argument>(
			[&m]() { _ConstView cv = m.make_const_view<D2, D2>(0, 6);  },
			"Invalid view column");
		test::check_exception<std::invalid_argument>(
			[&m]() { _ConstView cv = m.make_const_view<D2, D2>(5, 0);  },
			"Invalid view row");

		test::verbose("const Matrix::make_view: negative");
		test::check_exception<std::invalid_argument>(
			[&m]() { _ConstView cv = ((const _Matrix&)m).make_view<D2, D2>(0, 6);  },
			"Invalid view column");
		test::check_exception<std::invalid_argument>(
			[&m]() { _ConstView cv = ((const _Matrix&)m).make_view<D2, D2>(5, 0);  },
			"Invalid view row");
	}

	{
		test::verbose("Constructor tests: view: negative");
		test::check_exception<std::invalid_argument>(
			[&m]() { _View v(m, 0, 6);  },
			"Invalid view column");
		test::check_exception<std::invalid_argument>(
			[&m]() { _View v(m, 0, 4);  },
			"Invalid view column");
		test::check_exception<std::invalid_argument>(
			[&m]() { _View v(m, 5, 0);  },
			"Invalid view row");
		test::check_exception<std::invalid_argument>(
			[&m]() { _View v(m, 3, 0);  },
			"Invalid view row");

		test::verbose("Matrix::make_view: negative");
		test::check_exception<std::invalid_argument>(
			[&m]() { _View cv = m.make_view<D2, D2>(0, 6);  },
			"Invalid view column");
		test::check_exception<std::invalid_argument>(
			[&m]() { _View cv = m.make_view<D2, D2>(5, 0);  },
			"Invalid view row");
	}

	{
		test::verbose("Constructor tests: const view: positive");
		_ConstView cv1(m, 0, 0);
		_ConstView cv2(m, 0, 3);
		_ConstView cv3(m, 0, 0);
		_ConstView cv4(m, 2, 3);

		auto copy = cv4;
	}

	{
		test::verbose("Constructor tests: view: positive");
		_View v1(m, 0, 0);
		_View v2(m, 0, 3);
		_View v3(m, 0, 0);
		_View v4(m, 2, 3);

		auto copy = v3;
	}

	{
		_ConstView cv = m.make_const_view<D2, D2>(2, 1);
		_View v = m.make_view<D2, D2>(1, 2);

		test::verbose("Const view accessors");
		for (size_t row = 0; row < _ConstView::row_rank; ++row)
		{
			for (size_t col = 0; col < _ConstView::column_rank; ++col)
			{
				std::stringstream test;
				test << " for row=" << row << "; col=" << col;
				test::assert(m(row + 2, col + 1) == cv(row, col), ("Test Failed: m(row + 2, col + 1) == cv(row, col)" + test.str()).c_str());
			}

			test::check_exception<std::invalid_argument>(
				[&cv, row]() { cv(row, _ConstView::column_rank + 1);  },
				"Reading past row end: const view");
		}

		test::check_exception<std::invalid_argument>(
			[&cv]() { cv(_ConstView::row_rank, 0);  },
			"Reading past column end: const view");

		test::verbose("View accessors");
		for (size_t row = 0; row < _View::row_rank; ++row)
		{
			for (size_t col = 0; col < _View::column_rank; ++col)
			{
				std::stringstream test;
				test << " for row=" << row << "; col=" << col;
				test::assert(m(row + 1, col + 2) == v(row, col), ("Test Failed: m(row + 1, col + 2) == v(row, col)" + test.str()).c_str());
				test::assert(m(row + 1, col + 2) == ((const _View&)v)(row, col), ("Test Failed: m(row + 1, col + 2) == ((const _View&)v)(row, col)" + test.str()).c_str());

				double value = row * 1000.0 + 10.0 * col;
				test::assert(value == (v(row, col) = value), ("Test Failed: value == (v(row, col) = value)" + test.str()).c_str());
				test::assert(m(row + 1, col + 2) == value, ("Test Failed: m(row + 1, col + 2) == value" + test.str()).c_str());
			}

			test::check_exception<std::invalid_argument>(
				[&v, row]() { v(row, _View::column_rank + 1);  },
				"Reading past row end: view");
			test::check_exception<std::invalid_argument>(
				[&v, row]() { v(row, _View::column_rank + 1) = -5.0;  },
				"Writing past row end: view");
		}

		test::check_exception<std::invalid_argument>(
			[&v]() { v(_View::row_rank, 0);  },
			"Reading past column end: view");
		test::check_exception<std::invalid_argument>(
			[&v]() { v(_View::row_rank, 0) = -1.0;  },
			"Writing past column end: view");
	}

	sc.pass();
}