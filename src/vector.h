#pragma once

#include <vector>

#include "declaration.h"
#include "expression.h"

namespace algebra
{
	template <class _Vector>
	class const_vector_iterator
	{
	public:
		typedef typename _Vector::dimension dimension;
		static const size_t rank = dimension::rank;
		typedef const_vector_iterator<_Vector> _Self;

		typedef typename std::random_access_iterator_tag iterator_category;
		typedef typename const _Vector::value_type value_type;
		typedef typename std::ptrdiff_t difference_type;
		typedef difference_type distance_type;
		typedef typename const value_type* pointer;
		typedef typename const value_type& reference;

		const_vector_iterator(_Vector& vector)
			: m_pVector(std::addressof(vector)), m_index(0)
		{
		}

		const_vector_iterator(const _Self& other)
			: m_pVector(other.m_pVector), m_index(other.m_index)
		{}

		reference operator*() const
		{
			if (m_index >= _Self::rank)
				throw std::invalid_argument("Iterator cannot be dereferenced.");

			return (*(this->m_pVector))(m_index);
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
			return (this->m_pVector == other.m_pVector
				&& this->m_index == other.m_index);
		}

		bool operator!=(const _Self& other) const
		{
			return (!(*this == other));
		}

		bool operator<(const _Self& other) const
		{
			return (this->m_pVector < other.m_pVector
				|| (this->m_pVector == other.m_pVector
					&& this->m_index < other.m_index));
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
			if (m_index < _Self::rank)
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
				m_index = (offset <= (difference_type)(_Self::rank - m_index))
					? m_index + offset
					: _Self::rank;
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
		_Vector* m_pVector;
		size_t m_index;
	};

	// add offset to iterator
	template<class _Vector> inline
	const_vector_iterator<_Vector> operator+(
		typename const_vector_iterator<_Vector>::difference_type offset,
		const_vector_iterator<_Vector> right)
	{
		return (right += offset);
	}

	template <class _Vector>
	class vector_iterator : public const_vector_iterator<_Vector>
	{
	public:
		typedef typename _Vector::dimension dimension;
		static const size_t rank = dimension::rank;
		typedef const_vector_iterator<_Vector> _Base;
		typedef vector_iterator<_Vector> _Self;

		typedef typename _Vector::value_type value_type;
		typedef typename value_type* pointer;
		typedef typename value_type& reference;

		vector_iterator(_Vector& vector)
			: _Base(vector)
		{}

		vector_iterator(const _Self& other)
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
	template<class _Vector> inline
	vector_iterator<_Vector> operator+(
		typename vector_iterator<_Vector>::difference_type offset,
		vector_iterator<_Vector> right)
	{
		return (right += offset);
	}

	template<class _Vector>
	struct std::_Is_checked_helper<typename const_vector_iterator<_Vector> >
		: public std::true_type
	{	// mark const_vector_iterator as checked
	};

	template<class _Vector>
	struct std::_Is_checked_helper<typename vector_iterator<_Vector> >
		: public std::true_type
	{	// mark vector_iterator as checked
	};

	template <class D>
	class vector
	{
	private:

	public:
		typedef typename D dimension;
		static const size_t rank = dimension::rank;
		typedef vector<D> _Self;
		typedef double value_type;

		typedef typename const_vector_iterator<const _Self> const_vector_iterator;
		typedef typename vector_iterator<_Self> vector_iterator;

		static_assert(std::is_base_of<algebra::dimension<rank>, D>::value, "Type parameter D must be a dimension.");

		vector()
			: m_values()
		{
		}

		vector(std::initializer_list<value_type> data)
			: m_values()
		{
			if (data.size() != _Self::rank)
				throw std::invalid_argument("Initializer size does not match vector rank.");

			m_values.assign(data.begin(), data.end());
		}

		vector(const std::vector<value_type>& data)
			: m_values()
		{
			if (data.size() != _Self::rank)
				throw std::invalid_argument("Initializer size does not match vector rank.");

			m_values.assign(data.begin(), data.end());
		}

		vector(const _Self& other)
			: m_values(other.m_values)
		{}

		vector(_Self&& other)
			: m_values(std::move(other.m_values))
		{}

		_Self& operator= (const _Self& other)
		{
			if (this != std::addressof(other))
			{
				m_values = other.m_values;
			}

			return (*this);
		}

		_Self& operator= (_Self&& other)
		{
			if (this != std::addressof(other))
			{
				m_values = std::move(other.m_values);
			}

			return (*this);
		}

		value_type& operator() (const size_t index)
		{
			if (index >= _Self::rank)
				throw std::invalid_argument("Index out of range.");

			if (m_values.size() == 0)
			{
				const value_type zero = number_traits<value_type>::zero();
				m_values.assign(_Self::rank, zero);
			}

			return m_values[index];
		}

		const value_type& operator() (const size_t index) const
		{
			if (index >= _Self::rank)
				throw std::invalid_argument("Index out of range.");

			if (m_values.size() == 0)
			{
				static const value_type zero = number_traits<value_type>::zero();
				return zero;
			}

			return m_values[index];
		}

		bool equals(const _Self& other) const
		{
			for (size_t i = 0; i < _Self::rank; ++i)
			{
				if (false == number_traits<value_type>::equals((*this)(i), other(i)))
					return false;
			}

			return true;
		}

		bool empty() const
		{
			return this->m_values.size() == 0;
		}

		void clear()
		{
			this->m_values.clear();
		}

		const_vector_iterator begin() const
		{
			return const_vector_iterator(*this);
		}

		const_vector_iterator end() const
		{
			const_vector_iterator it(*this);
			it += _Self::rank;
			return it;
		}

		const_vector_iterator cbegin() const
		{
			return (((const _Self *)this)->begin());
		}

		const_vector_iterator cend() const
		{
			return (((const _Self *)this)->end());
		}

		vector_iterator begin()
		{
			return vector_iterator(*this);
		}

		vector_iterator end()
		{
			vector_iterator it(*this);
			it += _Self::rank;
			return it;
		}

		static _Self random(
			const value_type min = 0.0,
			const value_type max = 1.0)
		{
			_Self result;

			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<value_type> distr(min, max);

			result.m_values.reserve(_Self::rank);
			for (size_t i = 0; i < _Self::rank; i++)
			{
				result.m_values.push_back(distr(gen));
			}

			return result;
		}

	private:
		std::vector<value_type> m_values;
	};

	template <class D>
	bool operator== (
		const vector<D>& v1,
		const vector<D>& v2)
	{
		return v1.equals(v2);
	}

	template <class D>
	bool operator!= (
		const vector<D>& v1,
		const vector<D>& v2)
	{
		return !(v1 == v2);
	}

	template <class D>
	vector<D> operator+ (
		const vector<D>& v1,
		const vector<D>& v2)
	{
		vector<D> result;

		for (size_t i = 0; i < vector<D>::rank; ++i)
		{
			result(i) = v1(i) + v2(i);
		}

		return result;
	}

	template <class D>
	vector<D> operator- (
		const vector<D>& v1,
		const vector<D>& v2)
	{
		vector<D> result;

		for (size_t i = 0; i < vector<D>::rank; ++i)
		{
			result(i) = v1(i) - v2(i);
		}

		return result;
	}

	template <class D>
	vector<D> operator* (
		const vector<D>& v,
		const double C)
	{
		vector<D> result;

		for (size_t i = 0; i < vector<D>::rank; ++i)
		{
			result(i) = v(i) * C;
		}

		return result;
	}

	template <class D>
	vector<D> operator* (
		const double C,
		const vector<D>& v)
	{
		return v * C;
	}

	template <class D>
	double operator* (
		const vector<D>& v1,
		const vector<D>& v2)
	{
		double result = 0;

		for (size_t i = 0; i < vector<D>::rank; ++i)
		{
			result += v1(i) * v2(i);
		}

		return result;
	}

	namespace expressions
	{
		template <class D>
		expression<double> operator* (
			const expression<vector<D>> e1,
			const expression<vector<D>> e2)
		{
			return expression<double>([e1, e2]() { return e1.evaluate() * e2.evaluate(); });
		}

		template <class D>
		expression<double> operator* (
			const variable<vector<D>> v1,
			const expression<vector<D>> e2)
		{
			return expression<double>([v1, e2]() { return v1.value() * e2.evaluate(); });
		}

		template <class D>
		expression<double> operator* (
			const expression<vector<D>> e1,
			const variable<vector<D>> v2)
		{
			return expression<double>([e1, v2]() { return e1.evaluate() * v2.value(); });
		}

		template <class D>
		expression<double> operator* (
			const variable<vector<D>> v1,
			const variable<vector<D>> v2)
		{
			return expression<double>([v1, v2]() { return v1.value() * v2.value(); });
		}

		template <class D>
		expression<vector<D>> operator* (
			const expression<double> _scalar,
			const expression<vector<D>> _vector)
		{
			return expression<vector<D>>([_scalar, _vector]() { return _scalar.evaluate() * _vector.evaluate(); });
		}

		template <class D>
		expression<vector<D>> operator* (
			const expression<vector<D>> _vector,
			const expression<double> _scalar)
		{
			return expression<vector<D>>([_scalar, _vector]() { return _vector.evaluate() * _scalar.evaluate(); });
		}

		template <class D>
		expression<vector<D>> operator* (
			const expression<double> _scalar,
			const variable<vector<D>> _vector)
		{
			return expression<vector<D>>([_scalar, _vector]() { return _scalar.evaluate() * _vector.value(); });
		}

		template <class D>
		expression<vector<D>> operator* (
			const variable<vector<D>> _vector,
			const expression<double> _scalar)
		{
			return expression<vector<D>>([_scalar, _vector]() { return _vector.value() * _scalar.evaluate(); });
		}
	}
}