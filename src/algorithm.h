#pragma once

#include "matrix.h"

namespace algebra
{
	// Algorithm to solve a system of linear equations
	// defined by A * x = B
	template <class R>
	bool solve(
		const matrix<R, R>& a,
		const vector<R>& b,
		vector<R>& x)
	{
		typedef typename dimension<R::rank + 1> P;
		typedef typename matrix<R, P> solution;

		// Initialize solution matrix from the input
		// TODO: simplify using std::copy when column and vector iterators are implemented.
		solution m = a.resize<R, P>();
		for (size_t i = 0; i < R::rank; ++i)
		{
			m(i, R::rank) = b(i);
		}

		for (size_t col = 0; col < R::rank; ++col)
		{
			// All values in this row before the current column are zeros already, so skip them.
			auto itRow = m.crow_begin(col) + col, itRowEnd = m.crow_end(col);

			if (0.0 == *itRow)
				return false;

			// Add current row to all other rows in the matrix to make
			// all values in the current column zeros.
			for (size_t row = 0; row < R::rank; ++row)
			{
				if (row != col)
				{
					// All values of the main row before the current column are zeros already,
					// so skip them because it does not affect the end result.
					auto itCur = m.row_begin(row) + col;
					const double factor = -(*itCur) / (*itRow);

					// Transform corrent row by adding main row with the right multiplier.
					std::transform(itRow, itRowEnd, itCur, itCur,
						[&factor](const double& cur, const double& other) { return cur * factor + other; });
				}
			}
		}

		// To find the result, we now need to normalize each row in the solution matrix
		// and then copy the value of the last column into the result vector.
		// We can do that in a single operation.
		for (size_t row = 0; row < R::rank; ++row)
		{
			x(row) = m(row, R::rank) / m(row, row);
		}

		return true;
	}
}
