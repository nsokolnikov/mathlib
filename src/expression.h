#pragma once

#include <functional>
#include <memory>

namespace algebra
{
namespace expressions
{
	template <class T>
	class variable
	{
	public:
		typedef T type;

		variable(const variable<T>& other)
			: m_pvalue(other.m_pvalue)
		{}

		variable()
		{
			m_pvalue = std::make_shared<T>();
		}

		const T& value() const
		{
			return *m_pvalue;
		}

		T& value()
		{
			return *m_pvalue;
		}

		void set(const T& value)
		{
			*m_pvalue = value;
		}

		void set(T&& value)
		{
			*m_pvalue = std::move(value);
		}

	private:
		std::shared_ptr<T> m_pvalue;
	};

	template<class T>
	variable<T> declare()
	{
		return variable<T>();
	}

	template<class T>
	class expression
	{
	public:
		typedef typename std::function<T()> evaluator;

		expression() = delete;

		expression(const variable<T> var)
		{
			m_evaluator = std::make_shared<evaluator>([var]() { return var.value(); });
		}

		expression(const evaluator& func)
			: m_evaluator(std::make_shared<evaluator>(func))
		{
		}

		expression(evaluator&& func)
			: m_evaluator(std::make_shared<evaluator>(func))
		{
		}

		T evaluate() const
		{
			return (*m_evaluator)();
		}

	private:
		std::shared_ptr<evaluator> m_evaluator;
	};

	template <class T>
	expression<T> operator+ (
		const expression<T> e1,
		const expression<T> e2)
	{
		return expression<T>([e1, e2]() { return e1.evaluate() + e2.evaluate(); });
	}

	template <class T>
	expression<T> operator+ (
		const variable<T> v1,
		const variable<T> v2)
	{
		return expression<T>([v1, v2]() { return v1.value() + v2.value(); });
	}

	template <class T>
	expression<T> operator+ (
		const expression<T> e1,
		const variable<T> v2)
	{
		return expression<T>([e1, v2]() { return e1.evaluate() + v2.value(); });
	}

	template <class T>
	expression<T> operator+ (
		const variable<T> v1,
		const expression<T> e2)
	{
		return expression<T>([v1, e2]() { return v1.value() + e2.evaluate(); });
	}

	template <class T>
	expression<T> operator- (
		const expression<T> e1,
		const expression<T> e2)
	{
		return expression<T>([e1, e2]() { return e1.evaluate() - e2.evaluate(); });
	}

	template <class T>
	expression<T> operator- (
		const variable<T> v1,
		const variable<T> v2)
	{
		return expression<T>([v1, v2]() { return v1.value() - v2.value(); });
	}

	template <class T>
	expression<T> operator- (
		const expression<T> e1,
		const variable<T> v2)
	{
		return expression<T>([e1, v2]() { return e1.evaluate() - v2.value(); });
	}

	template <class T>
	expression<T> operator- (
		const variable<T> v1,
		const expression<T> e2)
	{
		return expression<T>([v1, e2](){ return v1.value() - e2.evaluate(); });
	}

	template <class T>
	expression<T> operator* (
		const variable<T> v,
		const double C)
	{
		return expression<T>([v, C]() { return (T)(v.value() * C); });
	}

	template <class T>
	expression<T> operator* (
		const double C,
		const variable<T> v)
	{
		return v * C;
	}

	template <class T>
	expression<T> operator* (
		const variable<T> v1,
		const variable<double> v2)
	{
		return expression<T>([v1, v2]() { return v1.value() * v2.value(); });
	}

	template <class T>
	expression<T> operator* (
		const variable<double> C,
		const variable<T> v)
	{
		return v * C;
	}

	template <class T>
	expression<T> operator* (
		const expression<T> e,
		const double C)
	{
		return expression<T>([e, C]() { return (T)(e.evaluate() * C); });
	}

	template <class T>
	expression<T> operator* (
		const double C,
		const expression<T> e)
	{
		return e * C;
	}

	template <class T>
	expression<T> operator* (
		const expression<T> e1,
		const expression<T> e2)
	{
		return expression<T>([e1, e2]() { return e1.evaluate() * e2.evaluate(); });
	}
}
}