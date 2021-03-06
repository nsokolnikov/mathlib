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
		typedef typename solution::value_type value_type;

		// Initialize solution matrix from the input and copy input vector
		// into the last column of the solution matrix.
		solution m = a.resize<R, P>();
		std::copy(b.begin(), b.end(), m.column_begin(R::rank));

		for (size_t col = 0; col < R::rank; ++col)
		{
			// All values in this row before the current column are zeros already, so skip them.
			auto itRow = m.crow_begin(col) + col, itRowEnd = m.crow_end(col);

			if (number_traits<value_type>::zero() == *itRow)
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
					const value_type factor = -(*itCur) / (*itRow);

					// Transform corrent row by adding main row with the right multiplier.
					std::transform(itRow, itRowEnd, itCur, itCur,
						[&factor](const value_type& cur, const value_type& other) { return cur * factor + other; });
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
