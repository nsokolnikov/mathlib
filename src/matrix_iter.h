

#include <vector>
#include <algorithm>
#include <random>

#include "declaration.h"
#include "expression.h"
#include "vector.h"

namespace algebra
{


	//column iterator
	template <class _Matrix>
	class const_column_iterator
	{
	public:
		typedef typename _Matrix::column_dimension row_dimension;
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
			if (m_column >= _Matrix::row_rank)
				throw std::invalid_argument("Column index is out of range.");
		}

		const_row_iterator(const _Self& other)
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
	

}
