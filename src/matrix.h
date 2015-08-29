#pragma once

#include <vector>
#include <algorithm>
#include <random>

#include "declaration.h"
#include "expression.h"
#include "vector.h"

namespace algebra
{
	template <class A, class B>
	struct _min_rank
	{
		typedef typename std::conditional<(A::rank <= B::rank), A, B>::type type;
	};
	
	//column iterator
	template <class _Matrix>
	class const_column_iterator
	{
	public:
		typedef typename _Matrix::row_dimension row_dimension;
		static const size_t row_rank = row_dimension::rank;
		typedef const_column_iterator<_Matrix> _Self;

		typedef typename std::random_access_iterator_tag iterator_category;
		typedef typename const _Matrix::value_type value_type;
		typedef typename std::ptrdiff_t difference_type;
		typedef difference_type distance_type;
		typedef typename const value_type* pointer;
		typedef typename const value_type& reference;

		const_column_iterator(_Matrix& matrix, const size_t column)
			: m_pMatrix(std::addressof(matrix)), m_column(column), m_index(0)
		{
			if (m_column >= _Matrix::column_rank)
				throw std::invalid_argument("Column index is out of range.");
		}

		const_column_iterator(const _Self& other)
			: m_pMatrix(other.m_pMatrix), m_column(other.m_column), m_index(other.m_index)
		{}

		reference operator*() const
		{
			if (m_index >= _Self::row_rank)
				throw std::invalid_argument("Iterator cannot be dereferenced.");

			return (*(this->m_pMatrix))(m_index, m_column);
		}

		// postincrement
		_Self operator++(int)
		{
			_Self t = *this;
			this->_Increment();
			return t;
		}

		// predincrement
		_Self& operator++()
		{
			this->_Increment();
			return (*this);
		}

		// postdecrement
		_Self operator--(int)
		{
			_Self t = *this;
			this->_Decrement();
			return t;
		}

		// predecrement
		_Self& operator--()
		{
			this->_Decrement();
			return (*this);
		}

		_Self& operator+=(difference_type offset)
		{
			this->_IncrementBy(offset);
			return (*this);
		}

		_Self& operator-=(difference_type offset)
		{
			this->_IncrementBy(-offset);
			return (*this);
		}

		bool operator==(const _Self& other) const
		{
			return (this->m_pMatrix == other.m_pMatrix
				&& this->m_column == other.m_column
				&& this->m_index == other.m_index);
		}

		bool operator!=(const _Self& other) const
		{
			return (!(*this == other));
		}

		bool operator<(const _Self& other) const
		{
			return (this->m_pMatrix < other.m_pMatrix
				|| (this->m_pMatrix == other.m_pMatrix
					&& (this->m_column < other.m_column
						|| (this->m_column == other.m_column
							&& this->m_index < other.m_index))));
		}

		bool operator>(const _Self& other) const
		{
			return (other < *this);
		}

		bool operator<=(const _Self& other) const
		{
			return (!(other < *this));
		}

		bool operator>=(const _Self& other) const
		{
			return (!(*this < other));
		}

		void _Increment()
		{
			if (m_index < _Self::row_rank)
			{
				++m_index;
			}
		}

		void _Decrement()
		{
			if (0 < m_index)
			{
				--m_index;
			}
		}

		void _IncrementBy(difference_type offset)
		{
			if (offset >= 0)
			{
				m_index = (offset <= (difference_type)(_Self::row_rank - m_index))
					? m_index + offset
					: _Self::row_rank;
			}
			else
			{
				m_index = ((difference_type)m_index > -offset) ? m_index + offset : 0;
			}
		}

		// return this + integer
		_Self operator+(difference_type offset) const
		{
			_Self t = *this;
			return (t += offset);
		}

		// return this - integer
		_Self operator-(difference_type offset) const
		{
			_Self t = *this;
			return (t -= offset);
		}

		// return difference of iterators
		difference_type operator-(const _Self& right) const
		{
			return (this->m_index - right.m_index);
		}

	protected:
		_Matrix* m_pMatrix;
		size_t m_column;
		size_t m_index;
	};

	template <class _Matrix>
	class column_iterator : public const_column_iterator<_Matrix>
	{
	public:
		typedef typename _Matrix::row_dimension row_dimension;
		static const size_t row_rank = row_dimension::rank;
		typedef const_column_iterator<_Matrix> _Base;
		typedef column_iterator<_Matrix> _Self;

		typedef typename _Matrix::value_type value_type;
		typedef typename value_type* pointer;
		typedef typename value_type& reference;

		column_iterator(_Matrix& matrix, const size_t row)
			: _Base(matrix, row)
		{}

		column_iterator(const _Self& other)
			: _Base(other)
		{}

		reference operator*() const
		{
			return (reference)**((_Base*)this);
		}

		// postincrement
		_Self operator++(int)
		{
			_Self t = *this;
			this->_Increment();
			return t;
		}

		// predecrement
		_Self& operator++()
		{
			this->_Increment();
			return (*this);
		}

		// postdecrement
		_Self operator--(int)
		{
			_Self t = *this;
			this->_Decrement();
			return t;
		}

		// predecrement
		_Self& operator--()
		{
			this->_Decrement();
			return (*this);
		}

		_Self& operator+=(difference_type offset)
		{
			this->_IncrementBy(offset);
			return (*this);
		}

		_Self& operator-=(difference_type offset)
		{
			this->_IncrementBy(-offset);
			return (*this);
		}

		// return this + integer
		_Self operator+(difference_type offset) const
		{
			_Self t = *this;
			return (t += offset);
		}

		// return this - integer
		_Self operator-(difference_type offset) const
		{
			_Self t = *this;
			return (t -= offset);
		}

		// return difference of iterators
		difference_type operator-(const _Self& right) const
		{
			return (this->m_index - right.m_index);
		}
	};

	//row iterator
	template <class _Matrix>
	class const_row_iterator
	{
	public:
		typedef typename _Matrix::column_dimension column_dimension;
		static const size_t column_rank = column_dimension::rank;
		typedef const_row_iterator<_Matrix> _Self;

		typedef typename std::random_access_iterator_tag iterator_category;
		typedef typename const _Matrix::value_type value_type;
		typedef typename std::ptrdiff_t difference_type;
		typedef difference_type distance_type;
		typedef typename const value_type* pointer;
		typedef typename const value_type& reference;

		const_row_iterator(_Matrix& matrix, const size_t row)
			: m_pMatrix(std::addressof(matrix)), m_row(row), m_index(0)
		{
			if (m_row >= _Matrix::row_rank)
				throw std::invalid_argument("Row index is out of range.");
		}

		const_row_iterator(const _Self& other)
			: m_pMatrix(other.m_pMatrix), m_row(other.m_row), m_index(other.m_index)
		{}

		reference operator*() const
		{
			if (m_index >= _Self::column_rank)
				throw std::invalid_argument("Iterator cannot be dereferenced.");

			return (*(this->m_pMatrix))(m_row, m_index);
		}

		// postincrement
		_Self operator++(int)
		{
			_Self t = *this;
			this->_Increment();
			return t;
		}

		// predincrement
		_Self& operator++()
		{
			this->_Increment();
			return (*this);
		}

		// postdecrement
		_Self operator--(int)
		{
			_Self t = *this;
			this->_Decrement();
			return t;
		}

		// predecrement
		_Self& operator--()
		{
			this->_Decrement();
			return (*this);
		}

		_Self& operator+=(difference_type offset)
		{
			this->_IncrementBy(offset);
			return (*this);
		}

		_Self& operator-=(difference_type offset)
		{
			this->_IncrementBy(-offset);
			return (*this);
		}

		bool operator==(const _Self& other) const
		{
			return (this->m_pMatrix == other.m_pMatrix
				&& this->m_row == other.m_row
				&& this->m_index == other.m_index);
		}

		bool operator!=(const _Self& other) const
		{
			return (!(*this == other));
		}

		bool operator<(const _Self& other) const
		{
			return (this->m_pMatrix < other.m_pMatrix
				|| (this->m_pMatrix == other.m_pMatrix
					&& (this->m_row < other.m_row
						|| (this->m_row == other.m_row
							&& this->m_index < other.m_index))));
		}

		bool operator>(const _Self& other) const
		{
			return (other < *this);
		}

		bool operator<=(const _Self& other) const
		{
			return (!(other < *this));
		}

		bool operator>=(const _Self& other) const
		{
			return (!(*this < other));
		}

		void _Increment()
		{
			if (m_index < _Self::column_rank)
			{
				++m_index;
			}
		}

		void _Decrement()
		{
			if (0 < m_index)
			{
				--m_index;
			}
		}

		void _IncrementBy(difference_type offset)
		{
			if (offset >= 0)
			{
				m_index = (offset <= (difference_type)(_Self::column_rank - m_index))
					? m_index + offset
					: _Self::column_rank;
			}
			else
			{
				m_index = ((difference_type)m_index > -offset) ? m_index + offset : 0;
			}
		}

		// return this + integer
		_Self operator+(difference_type offset) const
		{
			_Self t = *this;
			return (t += offset);
		}

		// return this - integer
		_Self operator-(difference_type offset) const
		{
			_Self t = *this;
			return (t -= offset);
		}

		// return difference of iterators
		difference_type operator-(const _Self& right) const
		{
			return (this->m_index - right.m_index);
		}

	protected:
		_Matrix* m_pMatrix;
		size_t m_row;
		size_t m_index;
	};

	template <class _Matrix>
	class row_iterator : public const_row_iterator<_Matrix>
	{
	public:
		typedef typename _Matrix::column_dimension column_dimension;
		static const size_t column_rank = column_dimension::rank;
		typedef const_row_iterator<_Matrix> _Base;
		typedef row_iterator<_Matrix> _Self;

		typedef typename _Matrix::value_type value_type;
		typedef typename value_type* pointer;
		typedef typename value_type& reference;

		row_iterator(_Matrix& matrix, const size_t row)
			: _Base(matrix, row)
		{}

		row_iterator(const _Self& other)
			: _Base(other)
		{}

		reference operator*() const
		{
			return (reference)**((_Base*)this);
		}

		// postincrement
		_Self operator++(int)
		{
			_Self t = *this;
			this->_Increment();
			return t;
		}

		// predecrement
		_Self& operator++()
		{
			this->_Increment();
			return (*this);
		}

		// postdecrement
		_Self operator--(int)
		{
			_Self t = *this;
			this->_Decrement();
			return t;
		}

		// predecrement
		_Self& operator--()
		{
			this->_Decrement();
			return (*this);
		}

		_Self& operator+=(difference_type offset)
		{
			this->_IncrementBy(offset);
			return (*this);
		}

		_Self& operator-=(difference_type offset)
		{
			this->_IncrementBy(-offset);
			return (*this);
		}

		// return this + integer
		_Self operator+(difference_type offset) const
		{
			_Self t = *this;
			return (t += offset);
		}

		// return this - integer
		_Self operator-(difference_type offset) const
		{
			_Self t = *this;
			return (t -= offset);
		}

		// return difference of iterators
		difference_type operator-(const _Self& right) const
		{
			return (this->m_index - right.m_index);
		}
	};

	// add offset to iterator
	template<class _Matrix> inline
	const_column_iterator<_Matrix> operator+(
		typename const_column_iterator<_Matrix>::difference_type offset,
		const_column_iterator<_Matrix> right)
	{
		return (right += offset);
	}

	// add offset to iterator
	template<class _Matrix> inline
	column_iterator<_Matrix> operator+(
		typename column_iterator<_Matrix>::difference_type offset,
		column_iterator<_Matrix> right)
	{
		return (right += offset);
	}

	// add offset to iterator
	template<class _Matrix> inline
		const_row_iterator<_Matrix> operator+(
		typename const_row_iterator<_Matrix>::difference_type offset,
		const_row_iterator<_Matrix> right)
	{
		return (right += offset);
	}

	// add offset to iterator
	template<class _Matrix> inline
		row_iterator<_Matrix> operator+(
		typename row_iterator<_Matrix>::difference_type offset,
		row_iterator<_Matrix> right)
	{
		return (right += offset);
	}

	template<class _Matrix>
	struct std::_Is_checked_helper<typename const_column_iterator<_Matrix> >
		: public std::true_type
	{	// mark const_column_iterator as checked
	};

	template<class _Matrix>
	struct std::_Is_checked_helper<typename column_iterator<_Matrix> >
		: public std::true_type
	{	// mark column_iterator as checked
	};

	template<class _Matrix>
	struct std::_Is_checked_helper<typename const_row_iterator<_Matrix> >
		: public std::true_type
	{	// mark const_row_iterator as checked
	};

	template<class _Matrix>
	struct std::_Is_checked_helper<typename row_iterator<_Matrix> >
		: public std::true_type
	{	// mark row_iterator as checked
	};

	template <class M, class N, class _Matrix>
	class const_view
	{
	public:
		typedef const_view<M, N, _Matrix> _Self;
		typedef typename M row_dimension;
		typedef typename N column_dimension;
		static const size_t row_rank = row_dimension::rank;
		static const size_t column_rank = column_dimension::rank;

		static_assert(std::is_base_of<dimension<row_rank>, M>::value, "Type parameter M must be a dimension.");
		static_assert(std::is_base_of<dimension<column_rank>, N>::value, "Type parameter N must be a dimension.");

		typedef typename _Matrix::value_type value_type;
		typedef typename const_row_iterator<const _Self> const_row_iterator;
		typedef typename const_column_iterator<const _Self> const_column_iterator;

		const_view(_Matrix& matrix, const size_t row, const size_t column)
			: m_pMatrix(std::addressof(matrix)), m_Row(row), m_Column(column)
		{
			if (column >= _Matrix::column_rank)
				throw std::invalid_argument("Base column index out of range.");
			if (_Matrix::column_rank - column < _Self::column_rank)
				throw std::invalid_argument("View is out of the base matrix column range.");

			if (row >= _Matrix::row_rank)
				throw std::invalid_argument("Base row index out of range.");
			if (_Matrix::row_rank - row < _Self::row_rank)
				throw std::invalid_argument("View is out of the base matrix row range.");
		}

		const_view(const _Self& other)
			: m_pMatrix(other.m_pMatrix), m_Row(other.m_Row), m_Column(other.m_Column)
		{}

		_Self operator=(const _Self& other) = delete;

		const value_type& operator() (
			const size_t row,
			const size_t column) const
		{
			if (column >= _Self::column_rank)
				throw std::invalid_argument("Column index out of range.");
			if (row >= _Self::row_rank)
				throw std::invalid_argument("Row index out of range.");

			return (*this->m_pMatrix)(m_Row + row, m_Column + column);
		}

		const_row_iterator row_begin(const size_t row)
		{
			return const_row_iterator(*this, row);
		}

		const_row_iterator row_end(const size_t row)
		{
			const_row_iterator it(*this, row);
			it += _Self::column_rank;
			return it;
		}

		const_column_iterator column_begin(const size_t column)
		{
			return const_column_iterator(*this, column);
		}

		const_column_iterator column_end(const size_t column) {
			const_column_iterator it(*this, column);
			it += _Self::row_rank;
			return it;
		}

	protected:
		_Matrix* m_pMatrix;
		const size_t m_Row;
		const size_t m_Column;
	};

	template <class M, class N, class _Matrix>
	class view : public const_view<M, N, _Matrix>
	{
	public:
		typedef typename const_view<M, N, _Matrix> _Base;
		typedef view<M, N, _Matrix> _Self;
		typedef typename M row_dimension;
		typedef typename N column_dimension;
		static const size_t row_rank = row_dimension::rank;
		static const size_t column_rank = column_dimension::rank;

		typedef typename _Matrix::value_type value_type;

		typedef typename row_iterator<_Self> row_iterator;
		typedef typename algebra::const_row_iterator<const _Self> const_row_iterator;

		typedef typename column_iterator<_Self> column_iterator;
		typedef typename algebra::const_column_iterator<const _Self> const_column_iterator;

		static_assert(std::is_base_of<dimension<row_rank>, M>::value, "Type parameter M must be a dimension.");
		static_assert(std::is_base_of<dimension<column_rank>, N>::value, "Type parameter N must be a dimension.");

		view(_Matrix& matrix, const size_t row, const size_t column)
			: _Base(matrix, row, column)
		{
		}

		view(const _Self& other)
			: _Base(other)
		{}

		value_type& operator() (
			const size_t row,
			const size_t column) const
		{
			return ((value_type&)(*(_Base*)this)(row, column));
		}

		row_iterator row_begin(const size_t row)
		{
			return row_iterator(*this, row);
		}

		row_iterator row_end(const size_t row)
		{
			row_iterator it(*this, row);
			it += _Self::column_rank;
			return it;
		}

		column_iterator column_begin(const size_t column)
		{
			return column_iterator(*this, column);
		}

		column_iterator column_end(const size_t column)
		{
			column_iterator it(*this, column);
			it += _Self::row_rank;
			return it;
		}

		const_row_iterator crow_begin(const size_t row) const
		{
			return const_row_iterator(*this, row);
		}

		const_row_iterator crow_end(const size_t row) const
		{
			const_row_iterator it(*this, row);
			it += _Self::column_rank;
			return it;
		}

		const_column_iterator ccolumn_begin(const size_t column) const
		{
			return const_column_iterator(*this, column);
		}

		const_column_iterator ccolumn_end(const size_t column) const
		{
			const_column_iterator it(*this, column);
			it += _Self::row_rank;
			return it;
		}
	};

	template <class M, class N>
	class matrix
	{
	public:
		typedef typename M row_dimension;
		typedef typename N column_dimension;
		typedef matrix<row_dimension, column_dimension> _Self;
		static const size_t row_rank = row_dimension::rank;
		static const size_t column_rank = column_dimension::rank;

		static_assert(std::is_base_of<dimension<row_rank>, M>::value, "Type parameter M must be a dimension.");
		static_assert(std::is_base_of<dimension<column_rank>, N>::value, "Type parameter N must be a dimension.");

		static_assert(_Self::column_rank <= 10000 && _Self::row_rank <= 10000, "Matrix dimensions cannot exceed 10000.");

		typedef double value_type;

		typedef typename const_column_iterator<const _Self> const_column_iterator;
		typedef typename column_iterator<_Self> column_iterator;
		typedef typename const_row_iterator<const _Self> const_row_iterator;
		typedef typename row_iterator<_Self> row_iterator;

		matrix()
			: m_values()
		{
		}

		matrix(const matrix& other)
			: m_values(other.m_values)
		{
		}

		matrix(matrix&& other)
			: m_values(std::move(other.m_values))
		{}

		matrix(std::initializer_list<value_type> data)
			: m_values()
		{
			if (data.size() != _Self::column_rank * _Self::row_rank)
				throw std::invalid_argument("Initializer size does not match matrix rank.");

			m_values.assign(data.begin(), data.end());
		}

		matrix(std::vector<value_type> data) {
			if (data.size() != _Self::column_rank * _Self::row_rank)
				throw std::invalid_argument("Initializer size does not match matrix rank.");

			m_values.assign(data.begin(), data.end());
		}

		bool empty() const
		{
			return m_values.size() == 0;
		}

		bool equals(const _Self& other) const
		{
			if (this == std::addressof(other))
				return true;

			if (this->empty())
			{
				if (other.empty())
					return true;

				return false;
			}
			else if (other.empty())
			{
				return false;
			}

			for (auto it = m_values.begin(), end = m_values.end(), oit = other.m_values.begin();
				it != end; ++it, ++oit)
			{
				if (false == number_traits<value_type>::equals(*it, *oit))
					return false;
			}

			return true;
		}

		_Self& operator=(const _Self& other)
		{
			if (this != std::addressof(other))
			{
				m_values = other.m_values;
			}

			return (*this);
		}

		_Self& operator=(_Self&& other)
		{
			if (this != std::addressof(other))
			{
				m_values = std::move(other.m_values);
			}

			return (*this);
		}

		value_type& operator() (
			const size_t row,
			const size_t column)
		{
			if (column >= _Self::column_rank)
				throw std::invalid_argument("Column index out of range.");
			if (row >= _Self::row_rank)
				throw std::invalid_argument("Row index out of range.");

			if (m_values.size() == 0)
			{
				const value_type zero = number_traits<value_type>::zero();
				m_values.assign(_Self::column_rank * _Self::row_rank, zero);
			}

			return m_values[row * _Self::column_rank + column];
		}

		const value_type& operator() (
			const size_t row,
			const size_t column) const
		{
			if (column >= _Self::column_rank)
				throw std::invalid_argument("Column index out of range.");
			if (row >= _Self::row_rank)
				throw std::invalid_argument("Row index out of range.");

			if (m_values.size() == 0)
			{
				static const value_type zero = 0.0;
				return zero;
			}

			return m_values[row * _Self::column_rank + column];
		}

		matrix<N, M> transpose()
		{
			matrix<N, M> result;

			if (false == this->empty())
			{
				for (size_t row = 0; row < _Self::row_rank; ++row)
				{
					for (size_t col = 0; col < _Self::column_rank; ++col)
					{
						result(col, row) = (*this)(row, col);
					}
				}
			}

			return result;
		}

		template <
			class _ViewRows,
			class _ViewColumns>
		typename const_view<_ViewRows, _ViewColumns, const _Self> make_const_view(
			const size_t row,
			const size_t column) const
		{
			static_assert(_ViewRows::rank <= _Self::row_rank, "Too many rows in view.");
			static_assert(_ViewColumns::rank <= _Self::column_rank, "Too many columns in view.");

			if (column >= _Self::column_rank)
				throw std::invalid_argument("Base column index is too large for the view.");
			if (row >= _Self::row_rank)
				throw std::invalid_argument("Base row index is too large for the view.");

			return const_view<_ViewRows, _ViewColumns, const _Self>(*this, row, column);
		}

		template <
			class _ViewRows,
			class _ViewColumns>
		typename const_view<_ViewRows, _ViewColumns, const _Self> make_view(
			const size_t row,
			const size_t column) const
		{
			return this->make_const_view<_ViewRows, _ViewColumns>(row, column);
		}

		template <
			class _ViewRows,
			class _ViewColumns>
		typename view<_ViewRows, _ViewColumns, _Self> make_view(
			const size_t row,
			const size_t column)
		{
			static_assert(_ViewRows::rank <= _Self::row_rank, "Too many rows in view.");
			static_assert(_ViewColumns::rank <= _Self::column_rank, "Too many columns in view.");

			if (column >= _Self::column_rank)
				throw std::invalid_argument("Base column index is too large for the view.");
			if (row >= _Self::row_rank)
				throw std::invalid_argument("Base row index is too large for the view.");

			return view<_ViewRows, _ViewColumns, _Self>(*this, row, column);
		}
		
		const_column_iterator column_begin(const size_t column) const
		{
			return const_column_iterator(*this, column);
		}
		
		const_column_iterator column_end(const size_t column) const
		{
			const_column_iterator it(*this, column);
			it += _Self::column_rank;
			return it;
		}

		const_column_iterator ccolumn_begin(const size_t column) const
		{
			return (((const _Self *)this)->column_begin(column));
		}

		const_column_iterator ccolumn_end(const size_t column) const
		{
			return (((const _Self *)this)->column_end(column));
		}

		column_iterator column_begin(const size_t column)
		{
			return column_iterator(*this, column);
		}

		column_iterator column_end(const size_t column)
		{
			column_iterator it(*this, column);
			it += _Self::column_rank;
			return it;
		}
			
		const_row_iterator row_begin(const size_t row) const
		{
			return const_row_iterator(*this, row);
		}

		const_row_iterator row_end(const size_t row) const
		{
			const_row_iterator it(*this, row);
			it += _Self::column_rank;
			return it;
		}

		const_row_iterator crow_begin(const size_t row) const
		{
			return (((const _Self *)this)->row_begin(row));
		}

		const_row_iterator crow_end(const size_t row) const
		{
			return (((const _Self *)this)->row_end(row));
		}
			
		row_iterator row_begin(const size_t row)
		{
			return row_iterator(*this, row);
		}

		row_iterator row_end(const size_t row)
		{
			row_iterator it(*this, row);
			it += _Self::column_rank;
			return it;
		}

		template <class A, class B>
		typename matrix<A, B> resize() const
		{
			matrix<A, B> result;

			if (false == this->empty())
			{
				const size_t rows = typename _min_rank<A, row_dimension>::type::rank;
				const size_t columns = typename _min_rank<B, column_dimension>::type::rank;

				for (size_t row = 0; row < rows; ++row)
				{
					for (size_t col = 0; col < columns; ++col)
					{
						result(row, col) = (*this)(row, col);
					}
				}
			}

			return result;
		}

		_Self& element_pow(const value_type C)
		{
			if (false == m_values.empty())
			{
				std::transform(
					m_values.cbegin(),
					m_values.cend(),
					m_values.begin(),
					[C](const value_type& d)
					{
						return std::pow(d, C);
					});
			}

			return (*this);
		}

		_Self& abs()
		{
			if (false == m_values.empty())
			{
				std::transform(
					m_values.cbegin(),
					m_values.cend(),
					m_values.begin(),
					[](const value_type& d) {return std::abs(d); });
			}

			return (*this);
		}

		value_type min() const
		{
			if (m_values.empty())
			{
				return number_traits<value_type>::zero();
			}

			value_type result = m_values[0];
			for (value_type t : m_values)
			{
				if (t < result) result = t;
			}

			return result;
		}

		value_type max() const
		{
			if (m_values.empty())
			{
				return number_traits<value_type>::zero();
			}

			value_type result = m_values[0];
			for (value_type t : m_values)
			{
				if (t > result) result = t;
			}

			return result;
		}

		value_type accumulate() const
		{
			value_type result = number_traits<value_type>::zero();
			for (value_type v : m_values)
			{
				result = result + v;
			}

			return result;
		}
		
		static _Self eye()
		{
			_Self result;
			const value_type _Zero = number_traits<value_type>::zero();

			result.m_values.reserve(_Self::row_rank * _Self::column_rank);
			for (size_t i = 0; i < _Self::row_rank * _Self::column_rank; ++i)
			{
				if (i % _Self::column_rank == i / _Self::column_rank)
				{
					result.m_values.push_back(1);
				}
				else
				{
					result.m_values.push_back(_Zero);
				}
			}

			return result;
		}

		static _Self ones()
		{
			const value_type _One = (number_traits<value_type>::zero() + 1);

			_Self result;
			result.m_values.reserve(_Self::row_rank * _Self::column_rank);
			for (size_t i = 0; i < _Self::row_rank * _Self::column_rank; ++i)
			{
				result.m_values.push_back(_One);
			}

			return result;
		}

		static _Self random(
			const value_type min = 0.0,
			const value_type max = 1.0)
		{
			_Self result;

			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<value_type> distr(min, max);

			result.m_values.reserve(_Self::row_rank * _Self::column_rank);
			for (size_t i = 0; i < _Self::row_rank * _Self::column_rank; i++)
			{
				result.m_values.push_back(distr(gen));
			}

			return result;
		}

		static _Self sum(
			const _Self& m1,
			const _Self& m2)
		{
			_Self result;

			if (false == m1.empty() && false == m2.empty())
			{
				result.m_values.reserve(_Self::row_rank * _Self::column_rank);

				auto im1 = m1.m_values.begin(),
					im2 = m2.m_values.begin(),
					im1end = m1.m_values.end();

				for (; im1 < im1end; ++im1, ++im2)
				{
					result.m_values.push_back((*im1) + (*im2));
				}
			}
			else if (false == m1.empty())
			{
				result = m1;
			}
			else if (false == m2.empty())
			{
				result = m2;
			}

			return result;
		}

		static _Self subtract(
			const _Self& m1,
			const _Self& m2)
		{
			_Self result;

			if (false == m1.empty())
			{
				if (false == m2.empty())
				{
					result.m_values.reserve(_Self::row_rank * _Self::column_rank);

					auto im1 = m1.m_values.begin(),
						im2 = m2.m_values.begin(),
						im1end = m1.m_values.end();

					for (; im1 < im1end; ++im1, ++im2)
					{
						result.m_values.push_back((*im1) - (*im2));
					}
				}
				else
				{
					result = m1;
				}
			}
			else if (false == m2.empty())
			{
				result = m2;
				std::transform(
					result.m_values.begin(),
					result.m_values.end(),
					result.m_values.begin(),
					[](const value_type& d) { return -d; });
			}

			return result;
		}

		static _Self multiply(
			const _Self& m,
			const value_type C)
		{
			_Self result;

			if (false == m.empty())
			{
				result = m;
				std::transform(
					result.m_values.begin(),
					result.m_values.end(),
					result.m_values.begin(),
					[C](const value_type& d) { return d * C; });
			}

			return result;
		}

		static _Self pow(
			const _Self& m,
			const size_t C)
		{
			_Self result;
			static_assert(_Self::column_rank == _Self::row_rank, "Cannot raise a non-square matrix to a power.");

			if (false == m.empty())
			{
				result = m;
				auto c = C - 1;
				while (c > 0)
				{
					result = result * m;
					c--;
				}
			}

			return result;
		}

	private:
		std::vector<value_type> m_values;
	};

	template <class M, class N, class P>
	matrix<M, P> operator* (
		const matrix<M, N>& m1,
		const matrix<N, P>& m2)
	{
		matrix<M, P> result;

		if (false == m1.empty() && false == m2.empty())
		{
			for (size_t row = 0; row < M::rank; ++row)
			{
				for (size_t col = 0; col < P::rank; ++col)
				{
					typename matrix<M, N>::value_type cell = number_traits<typename matrix<M, N>::value_type>::zero();

					for (size_t i = 0; i < N::rank; ++i)
					{
						cell += m1(row, i) * m2(i, col);
					}

					result(row, col) = cell;
				}
			}
		}

		return result;
	}

	template <class M, class N>
	matrix<M, N> operator+ (
		const matrix<M, N>& m1,
		const matrix<M, N>& m2)
	{
		return matrix<M, N>::sum(m1, m2);
	}

	template <class M, class N>
	matrix<M, N> operator- (
		const matrix<M, N>& m1,
		const matrix<M, N>& m2)
	{
		return matrix<M, N>::subtract(m1, m2);
	}

	template <class M, class N>
	matrix<M, N> operator* (
		const matrix<M, N>& m,
		const double C)
	{
		return matrix<M, N>::multiply(m, C);
	}

	template <class M, class N>
	matrix<M, N> operator* (
		const double C,
		const matrix<M, N>& m)
	{
		return matrix<M, N>::multiply(m, C);
	}

	template <class M>
	matrix<M, M> operator^ (
		const matrix<M, M>& m,
		const size_t C)
	{
		return matrix<M, M>::pow(m, C);
	}

	template <class M, class N>
	vector<M> operator* (
		const matrix<M, N>& m,
		const vector<N>& v)
	{
		vector<M> result;

		if (false == m.empty() && false == v.empty())
		{
			for (size_t row = 0; row < M::rank; ++row)
			{
				typename matrix<M, N>::value_type cell = number_traits<typename matrix<M, N>::value_type>::zero();
				for (size_t col = 0; col < N::rank; ++col)
				{
					cell += m(row, col) * v(col);
				}

				result(row) = cell;
			}
		}

		return result;
	}

	template <class A, class B>
	bool operator== (
		const matrix<A, B>& m1,
		const matrix<A, B>& m2)
	{
		return m1.equals(m2);
	}

	template <class A, class B>
	bool operator!= (
		const matrix<A, B>& m1,
		const matrix<A, B>& m2)
	{
		return !(m1 == m2);
	}

	// Computation for cost of multiplication of two matrices.
	template <class A, class B, class C>
	struct _2_matrix_mul_cost
	{
		static const size_t cost = A::rank * B::rank * C::rank;

		typedef typename matrix<A, C> result_type;
		typedef typename _2_matrix_mul_cost<A, B, C> type;

		static matrix<A, C>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2)
		{
			return m1 * m2;
		}
	};

	// Three-matrix chain multiplication: option 1.
	template <class A, class B, class C, class D>
	struct _3_matrix_mul_impl_1
	{
		typedef typename _2_matrix_mul_cost<A, B, C> temp_result;
		typedef typename _2_matrix_mul_cost<
			typename temp_result::result_type::row_dimension,
			typename temp_result::result_type::column_dimension, D> final_result;

		static const size_t cost = typename temp_result::cost + typename final_result::cost;

		static_assert((cost == (A::rank * C::rank * (B::rank + D::rank))), "Cost computation error");

		typedef typename final_result::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, D>>::value,
			"Final result computation error");

		static matrix<A, D>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2,
			const matrix<C, D>& m3)
		{
			return (m1 * m2) * m3;
		}
	};

	// Three-matrix chain multiplication: option 2.
	template <class A, class B, class C, class D>
	struct _3_matrix_mul_impl_2
	{
		typedef typename _2_matrix_mul_cost<B, C, D> temp_result;
		typedef typename _2_matrix_mul_cost<
			A,
			typename temp_result::result_type::row_dimension,
			typename temp_result::result_type::column_dimension> final_result;

		static const size_t cost = typename temp_result::cost + typename final_result::cost;

		static_assert(cost == (A::rank + C::rank) * B::rank * D::rank, "Cost computation error");

		typedef typename final_result::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, D>>::value,
			"Final result computation error");

		static matrix<A, D>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2,
			const matrix<C, D>& m3)
		{
			return m1 * (m2 * m3);
		}
	};

	// Utility template to select best plan for 3-matrix chain.
	template <class A, class B, class C, class D>
	struct _3_matix_mul_best_cost
	{
		typedef typename _3_matrix_mul_impl_1<A, B, C, D> impl_1;
		typedef typename _3_matrix_mul_impl_1<A, B, C, D> impl_2;

		typedef typename min_cost<impl_1, impl_2>::type type;

		typedef typename type::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, D>>::value,
			"Final result computation error");

		static const size_t cost = type::cost;
	};

	// Four-matrix chain multiplication: option 1.
	template <class A, class B, class C, class D, class E>
	struct _4_matrix_mul_impl_1
	{
		typedef typename _3_matix_mul_best_cost<B, C, D, E> temp_result;
		typedef typename _2_matrix_mul_cost<
			A,
			typename temp_result::result_type::row_dimension,
			typename temp_result::result_type::column_dimension> final_result;

		static const size_t cost = typename temp_result::cost + typename final_result::cost;

		typedef typename final_result::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, E>>::value,
			"Final result computation error");

		static matrix<A, E>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2,
			const matrix<C, D>& m3,
			const matrix<D, E>& m4)
		{
			return m1 * temp_result::type::multiply(m2, m3, m4);
		}
	};

	// Four-matrix chain multiplication: option 2.
	template <class A, class B, class C, class D, class E>
	struct _4_matrix_mul_impl_2
	{
		typedef typename _3_matix_mul_best_cost<A, B, C, D> temp_result;
		typedef typename _2_matrix_mul_cost<
			typename temp_result::result_type::row_dimension,
			typename temp_result::result_type::column_dimension,
			E> final_result;

		static const size_t cost = typename temp_result::cost + typename final_result::cost;

		typedef typename final_result::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, E>>::value,
			"Final result computation error");

		static matrix<A, E>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2,
			const matrix<C, D>& m3,
			const matrix<D, E>& m4)
		{
			return temp_result::type::multiply(m1, m2, m3) * m4;
		}
	};

	// Four-matrix chain multiplication: option 3.
	template <class A, class B, class C, class D, class E>
	struct _4_matrix_mul_impl_3
	{
		typedef typename _2_matrix_mul_cost<A, B, C> temp_result_1;
		typedef typename _2_matrix_mul_cost<C, D, E> temp_result_2;
		typedef typename _2_matrix_mul_cost<
			typename temp_result_1::result_type::row_dimension,
			typename temp_result_1::result_type::column_dimension,
			typename temp_result_2::result_type::column_dimension> final_result;

		static const size_t cost = typename temp_result_1::cost + typename temp_result_2::cost + typename final_result::cost;

		typedef typename final_result::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, E>>::value,
			"Final result computation error");

		static matrix<A, E>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2,
			const matrix<C, D>& m3,
			const matrix<D, E>& m4)
		{
			return (m1 * m2) * (m3 * m4);
		}
	};

	// Utility template to select best plan for 4-matrix chain.
	template <class A, class B, class C, class D, class E>
	struct _4_matix_mul_best_cost
	{
		typedef typename _4_matrix_mul_impl_1<A, B, C, D, E> impl_1;
		typedef typename _4_matrix_mul_impl_2<A, B, C, D, E> impl_2;
		typedef typename _4_matrix_mul_impl_3<A, B, C, D, E> impl_3;

		typedef typename min_cost<impl_1, impl_2, impl_3>::type type;

		typedef typename type::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, E>>::value,
			"Final result computation error");

		static const size_t cost = type::cost;
	};

	// Five-matrix chain multiplication: option 1.
	template <class A, class B, class C, class D, class E, class F>
	struct _5_matrix_mul_impl_1
	{
		typedef typename _4_matix_mul_best_cost<B, C, D, E, F> temp_result;
		typedef typename _2_matrix_mul_cost<
			A,
			typename temp_result::result_type::row_dimension,
			typename temp_result::result_type::column_dimension> final_result;

		static const size_t cost = typename temp_result::cost + typename final_result::cost;

		typedef typename final_result::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, F>>::value,
			"Final result computation error");

		static matrix<A, F>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2,
			const matrix<C, D>& m3,
			const matrix<D, E>& m4,
			const matrix<E, F>& m5)
		{
			return m1 * temp_result::type::multiply(m2, m3, m4, m5);
		}
	};

	// Five-matrix chain multiplication: option 2.
	template <class A, class B, class C, class D, class E, class F>
	struct _5_matrix_mul_impl_2
	{
		typedef typename _4_matix_mul_best_cost<A, B, C, D, E> temp_result;
		typedef typename _2_matrix_mul_cost<
			typename temp_result::result_type::row_dimension,
			typename temp_result::result_type::column_dimension,
			F> final_result;

		static const size_t cost = typename temp_result::cost + typename final_result::cost;

		typedef typename final_result::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, F>>::value,
			"Final result computation error");

		static matrix<A, F>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2,
			const matrix<C, D>& m3,
			const matrix<D, E>& m4,
			const matrix<E, F>& m5)
		{
			return temp_result::type::multiply(m1, m2, m3, m4) * m5;
		}
	};

	// Five-matrix chain multiplication: option 3.
	template <class A, class B, class C, class D, class E, class F>
	struct _5_matrix_mul_impl_3
	{
		typedef typename _3_matix_mul_best_cost<A, B, C, D> temp_result;
		typedef typename _3_matix_mul_best_cost<
			typename temp_result::result_type::row_dimension,
			typename temp_result::result_type::column_dimension,
			E,
			F> final_result;

		static const size_t cost = typename temp_result::cost + typename final_result::cost;

		typedef typename final_result::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, F>>::value,
			"Final result computation error");

		static matrix<A, F>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2,
			const matrix<C, D>& m3,
			const matrix<D, E>& m4,
			const matrix<E, F>& m5)
		{
			return final_result::type::multiply(
				temp_result::type::multiply(m1, m2, m3),
				m4,
				m5);
		}
	};

	// Five-matrix chain multiplication: option 4.
	template <class A, class B, class C, class D, class E, class F>
	struct _5_matrix_mul_impl_4
	{
		typedef typename _3_matix_mul_best_cost<C, D, E, F> temp_result;
		typedef typename _3_matix_mul_best_cost<
			A,
			B,
			typename temp_result::result_type::row_dimension,
			typename temp_result::result_type::column_dimension> final_result;

		static const size_t cost = typename temp_result::cost + typename final_result::cost;

		typedef typename final_result::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, F>>::value,
			"Final result computation error");

		static matrix<A, F>
		multiply(
			const matrix<A, B>& m1,
			const matrix<B, C>& m2,
			const matrix<C, D>& m3,
			const matrix<D, E>& m4,
			const matrix<E, F>& m5)
		{
			return final_result::type::multiply(m1, m2, temp_result::type::multiply(m3, m4, m5));
		}
	};

	// Utility template to select best plan for 5-matrix chain.
	template <class A, class B, class C, class D, class E, class F>
	struct _5_matix_mul_best_cost
	{
		typedef typename _5_matrix_mul_impl_1<A, B, C, D, E, F> impl_1;
		typedef typename _5_matrix_mul_impl_2<A, B, C, D, E, F> impl_2;
		typedef typename _5_matrix_mul_impl_3<A, B, C, D, E, F> impl_3;
		typedef typename _5_matrix_mul_impl_4<A, B, C, D, E, F> impl_4;

		typedef typename min_cost<impl_1, impl_2, impl_3, impl_4>::type type;

		typedef typename type::result_type result_type;

		static_assert(
			std::is_same<result_type, typename matrix<A, F>>::value,
			"Final result computation error");

		static const size_t cost = type::cost;
	};

	// Utility template to select execution plan for arbitrary chain of matrices.
	// Is not guarantied to pick the best possible plan, because it does not consider
	// all possible options, but is relatively good at picking reasonable plans.
	template <class M1, class M2, class M3, class... _Args>
	struct _matrix_mul_best_cost
	{
		static_assert(M1::column_dimension::rank == M2::row_dimension::rank, "Row rank of M1 does not match column rank of M2.");

		typedef typename _2_matrix_mul_cost<
			typename M1::row_dimension,
			typename M1::column_dimension,
			typename _matrix_mul_best_cost<M2, M3, _Args...>::result_type::column_dimension> _1_x_args;

		struct impl_1 : public _1_x_args
		{
			static const size_t cost = _1_x_args::cost + typename _matrix_mul_best_cost<M2, M3, _Args...>::cost;

			static typename _1_x_args::result_type
			multiply(
				const M1& m1,
				const M2& m2,
				const M3& m3,
				const _Args&... args)
			{
				return m1 * _matrix_mul_best_cost<M2, M3, _Args...>::type::multiply(m2, m3, args ...);
			}
		};

		typedef typename _3_matix_mul_best_cost<
			typename M1::row_dimension,
			typename M1::column_dimension,
			typename M2::column_dimension,
			typename _matrix_mul_best_cost<M3, _Args...>::result_type::column_dimension> _2_x_args;

		struct impl_2 : public _2_x_args
		{
			static const size_t cost = _2_x_args::cost + typename _matrix_mul_best_cost<M3, _Args...>::cost;

			static typename _2_x_args::result_type
			multiply(
				const M1& m1,
				const M2& m2,
				const M3& m3,
				const _Args&... args)
			{
				return _2_x_args::type::multiply(
					m1, m2,
					_matrix_mul_best_cost<M3, _Args...>::type::multiply(m3, args ...));
			}
		};

		typedef typename _4_matix_mul_best_cost<
			typename M1::row_dimension,
			typename M1::column_dimension,
			typename M2::column_dimension,
			typename M3::column_dimension,
			typename _matrix_mul_best_cost<_Args...>::result_type::column_dimension> _3_x_args;

		struct impl_3 : public _3_x_args
		{
			static const size_t cost = _3_x_args::cost + typename _matrix_mul_best_cost<_Args...>::cost;

			static typename _3_x_args::result_type
			multiply(
				const M1& m1,
				const M2& m2,
				const M3& m3,
				const _Args&... args)
			{
				return _3_x_args::type::multiply(
					m1, m2, m3,
					_matrix_mul_best_cost<_Args...>::type::multiply(args ...));
			}
		};

		typedef typename min_cost<impl_1, impl_2, impl_3>::type type;

		typedef typename type::result_type result_type;

		static const size_t cost = type::cost;
	};

	template <class M1, class M2, class M3>
	struct _matrix_mul_best_cost<M1, M2, M3>
	{
		static_assert(M1::column_dimension::rank == M2::row_dimension::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimension::rank == M3::row_dimension::rank, "Row rank of M2 does not match column rank of M3.");

		typedef typename _3_matix_mul_best_cost<
			typename M1::row_dimension,
			typename M1::column_dimension,
			typename M2::column_dimension,
			typename M3::column_dimension>::type type;

		typedef typename type::result_type result_type;

		static const size_t cost = type::cost;
	};

	template <class M1, class M2, class M3, class M4>
	struct _matrix_mul_best_cost<M1, M2, M3, M4>
	{
		static_assert(M1::column_dimension::rank == M2::row_dimension::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimension::rank == M3::row_dimension::rank, "Row rank of M2 does not match column rank of M3.");
		static_assert(M3::column_dimension::rank == M4::row_dimension::rank, "Row rank of M3 does not match column rank of M4.");

		typedef typename _4_matix_mul_best_cost<
			typename M1::row_dimension,
			typename M1::column_dimension,
			typename M2::column_dimension,
			typename M3::column_dimension,
			typename M4::column_dimension>::type type;

		typedef typename type::result_type result_type;

		static const size_t cost = type::cost;
	};

	template <class M1, class M2, class M3, class M4, class M5>
	struct _matrix_mul_best_cost<M1, M2, M3, M4, M5>
	{
		static_assert(M1::column_dimension::rank == M2::row_dimension::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimension::rank == M3::row_dimension::rank, "Row rank of M2 does not match column rank of M3.");
		static_assert(M3::column_dimension::rank == M4::row_dimension::rank, "Row rank of M3 does not match column rank of M4.");
		static_assert(M4::column_dimension::rank == M5::row_dimension::rank, "Row rank of M4 does not match column rank of M5.");

		typedef typename _5_matix_mul_best_cost<
			typename M1::row_dimension,
			typename M1::column_dimension,
			typename M2::column_dimension,
			typename M3::column_dimension,
			typename M4::column_dimension,
			typename M5::column_dimension>::type type;

		typedef typename type::result_type result_type;

		static const size_t cost = type::cost;
	};

	template <class M1, class M2>
	matrix<
		typename M1::row_dimension,
		typename M2::column_dimension>
	multiply(
		const M1& m1,
		const M2& m2)
	{
		static_assert(M1::column_dimension::rank == M2::row_dimension::rank, "Row rank of M1 does not match column rank of M2.");

		return m1 * m2;
	}

	template <class M1, class M2, class M3>
	matrix<
		typename M1::row_dimension,
		typename M3::column_dimension>
	multiply(
		const M1& m1,
		const M2& m2,
		const M3& m3)
	{
		static_assert(M1::column_dimension::rank == M2::row_dimension::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimension::rank == M3::row_dimension::rank, "Row rank of M2 does not match column rank of M3.");

		return _3_matix_mul_best_cost<
			typename M1::row_dimension,
			typename M1::column_dimension,
			typename M2::column_dimension,
			typename M3::column_dimension>::type::multiply(m1, m2, m3);
	}

	// Variadic template for chain multiplication of 4 or more matrices.
	template <class M1, class M2, class M3, class... _Args>
	matrix<
		typename M1::row_dimension,
		typename last<_Args...>::type::column_dimension>
	multiply(
		const M1& m1,
		const M2& m2,
		const M3& m3,
		const _Args&... args)
	{
		static_assert(M1::column_dimension::rank == M2::row_dimension::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimension::rank == M3::row_dimension::rank, "Row rank of M2 does not match column rank of M3.");

		return _matrix_mul_best_cost<M1, M2, M3, _Args...>::type::multiply(m1, m2, m3, args...);
	}

	namespace expressions
	{
		template <class M, class N, class P>
		expression<matrix<M, P>> operator* (
			const expression<matrix<M, N>> e1,
			const expression<matrix<N, P>> e2)
		{
			return expression<matrix<M, P>>(
				[e1, e2]() { return e1.evaluate() * e2.evaluate(); });
		}

		template <class M, class N, class P>
		expression<matrix<M, P>> operator* (
			const variable<matrix<M, N>> v1,
			const expression<matrix<N, P>> e2)
		{
			return expression<matrix<M, P>>(
				[v1, e2]() { return v1.value() * e2.evaluate(); });
		}

		template <class M, class N, class P>
		expression<matrix<M, P>> operator* (
			const expression<matrix<M, N>> e1,
			const variable<matrix<N, P>> v2)
		{
			return expression<matrix<M, P>>(
				[e1, v2]() { return e1.evaluate() * v2.value(); });
		}

		template <class M, class N, class P>
		expression<matrix<M, P>> operator* (
			const variable<matrix<M, N>> v1,
			const variable<matrix<N, P>> v2)
		{
			return expression<matrix<M, P>>(
				[v1, v2]() { return v1.value() * v2.value(); });
		}

		template <class M, class N>
		expression<matrix<M, N>> operator* (
			const expression<matrix<M, N>> e1,
			const expression<double> e2)
		{
			return expression<matrix<M, N>>(
				[e1, e2]() { return e1.evaluate() * e2.evaluate(); });
		}

		template <class M, class N>
		expression<matrix<M, N>> operator* (
			const expression<double> e1,
			const expression<matrix<M, N>> e2)
		{
			return expression<matrix<M, N>>(
				[e1, e2]() { return e1.evaluate() * e2.evaluate(); });
		}

		template <class M, class N>
		expression<vector<M>> operator* (
			const expression<matrix<M, N>> e1,
			const expression<vector<N>> e2)
		{
			return expression<vector<M>>(
				[e1, e2]() { return e1.evaluate() * e2.evaluate(); });
		}

		template <class M, class N>
		expression<vector<M>> operator* (
			const expression<matrix<M, N>> e1,
			const variable<vector<N>> v2)
		{
			return expression<vector<M>>(
				[e1, v2]() { return e1.evaluate() * v2.value(); });
		}

		template <class M, class N>
		expression<vector<M>> operator* (
			const variable<matrix<M, N>> v1,
			const expression<vector<N>> e2)
		{
			return expression<vector<M>>(
				[v1, e2]() { return v2.value() * e2.evaluate(); });
		}

		template <class M, class N>
		expression<vector<M>> operator* (
			const variable<matrix<M, N>> v1,
			const variable<vector<N>> v2)
		{
			return expression<vector<M>>(
				[v1, v2]() { return v2.value() * v2.value(); });
		}

		template <class M, class N>
		expression<matrix<N, M>> transpose(
			const expression<matrix<M, N>> e)
		{
			return expression<matrix<N, M>>(
				[e]() { return e.evaluate().transpose(); });
		}
	}
}