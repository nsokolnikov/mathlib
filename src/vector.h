#pragma once

#include <vector>

#include "declaration.h"
#include "expression.h"

namespace algebra
{
	template <class D>
	class vector
	{
	private:

	public:
		typedef typename D dimention;
		static const size_t rank = dimention::rank;
		typedef vector<D> _Self;

		static_assert(std::is_base_of<algebra::dimention<rank>, D>::value, "Type parameter D must be a dimention.");

		vector()
			: m_values()
		{
		}

		vector(std::initializer_list<double> data)
			: m_values()
		{
			if (data.size() != _Self::rank)
				throw std::invalid_argument("Initializer size does not match vector rank.");

			m_values.assign(data.begin(), data.end());
		}

		vector(const _Self& other)
			: m_values(other.m_values)
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

		double& operator() (const size_t index)
		{
			if (index >= _Self::rank)
				throw std::invalid_argument("Index out of range.");

			if (m_values.size() == 0)
			{
				const double zero = 0.0;
				m_values.assign(_Self::rank, zero);
			}

			return m_values[index];
		}

		const double& operator() (const size_t index) const
		{
			if (index >= _Self::rank)
				throw std::invalid_argument("Index out of range.");

			if (m_values.size() == 0)
			{
				static const double zero = 0.0;
				return zero;
			}

			return m_values[index];
		}

		bool equals(const _Self& other) const
		{
			for (size_t i = 0; i < _Self::rank; ++i)
			{
				if ((*this)(i) != other(i))
					return false;
			}

			return true;
		}

		bool empty() const
		{
			return this->m_values.size() == 0;
		}

	private:
		std::vector<double> m_values;
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