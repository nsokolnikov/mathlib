#pragma once

#include <vector>
#include <algorithm>
#include <random>

#include "declaration.h"
#include "expression.h"
#include "vector.h"

namespace algebra
{
	template <class M, class N>
	class matrix
	{
	public:
		typedef typename M row_dimention;
		typedef typename N column_dimention;
		typedef matrix<row_dimention, column_dimention> _Self;
		static const size_t row_rank = row_dimention::rank;
		static const size_t column_rank = column_dimention::rank;

		static_assert(std::is_base_of<dimension<row_rank>, M>::value, "Type parameter M must be a dimention.");
		static_assert(std::is_base_of<dimension<column_rank>, N>::value, "Type parameter N must be a dimention.");

		static_assert(_Self::column_rank <= 10000 && _Self::row_rank <= 10000, "Matrix dimentions cannot exceed 10000.");

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

		matrix(std::initializer_list<double> data)
			: m_values()
		{
			if (data.size() != _Self::column_rank * _Self::row_rank)
				throw std::invalid_argument("Initializer size does not match matrix rank.");

			m_values.assign(data.begin(), data.end());
		}

		bool empty() const
		{
			return m_values.size() == 0;
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

		double& operator() (
			const size_t row,
			const size_t column)
		{
			if (column >= _Self::column_rank)
				throw std::invalid_argument("Column index out of range.");
			if (row >= _Self::row_rank)
				throw std::invalid_argument("Row index out of range.");

			if (m_values.size() == 0)
			{
				const double zero = 0.0;
				m_values.assign(_Self::column_rank * _Self::row_rank, zero);
			}

			return m_values[row * _Self::column_rank + column];
		}

		const double& operator() (
			const size_t row,
			const size_t column) const
		{
			if (column >= _Self::column_rank)
				throw std::invalid_argument("Column index out of range.");
			if (row >= _Self::row_rank)
				throw std::invalid_argument("Row index out of range.");

			if (m_values.size() == 0)
			{
				static const double zero = 0.0;
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

		static _Self sum(const _Self& m1, const _Self& m2)
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

		static _Self subtract(const _Self& m1, const _Self& m2)
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
					[](const double& d) { return -d; });
			}

			return result;
		}

		static _Self multiply(const _Self& m, const double C)
		{
			_Self result;

			if (false == m.empty())
			{
				result = m;
				std::transform(
					result.m_values.begin(),
					result.m_values.end(),
					result.m_values.begin(),
					[C](const double& d) { return d * C; });
			}

			return result;
		}

		static _Self random(
			const double min = 0.0,
			const double max = 1.0)
		{
			_Self result;

			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> distr(min, max);

			result.m_values.reserve(_Self::row_rank * _Self::column_rank);
			for (size_t i = 0; i < _Self::row_rank * _Self::column_rank; i++)
			{
				result.m_values.push_back(distr(gen));
			}

			return result;
		}

	private:
		std::vector<double> m_values;
	};

	template <class M, class N, class P>
	matrix<M, P> operator* (const matrix<M, N>& m1, const matrix<N, P>& m2)
	{
		matrix<M, P> result;

		if (false == m1.empty() && false == m2.empty())
		{
			for (size_t row = 0; row < M::rank; ++row)
			{
				for (size_t col = 0; col < P::rank; ++col)
				{
					double cell = 0.0;

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
	matrix<M, N> operator* (const matrix<M, N>& m, const double C)
	{
		return matrix<M, N>::multiply(m, C);
	}

	template <class M, class N>
	matrix<M, N> operator* (const double C, const matrix<M, N>& m)
	{
		return matrix<M, N>::multiply(m, C);
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
				double cell = 0.0;
				for (size_t col = 0; col < N::rank; ++col)
				{
					cell += m(row, col) * v(col);
				}

				result(row) = cell;
			}
		}

		return result;
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
			typename temp_result::result_type::row_dimention,
			typename temp_result::result_type::column_dimention, D> final_result;

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
			typename temp_result::result_type::row_dimention,
			typename temp_result::result_type::column_dimention> final_result;

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
			typename temp_result::result_type::row_dimention,
			typename temp_result::result_type::column_dimention> final_result;

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
			typename temp_result::result_type::row_dimention,
			typename temp_result::result_type::column_dimention,
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
			typename temp_result_1::result_type::row_dimention,
			typename temp_result_1::result_type::column_dimention,
			typename temp_result_2::result_type::column_dimention> final_result;

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
			typename temp_result::result_type::row_dimention,
			typename temp_result::result_type::column_dimention> final_result;

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
			typename temp_result::result_type::row_dimention,
			typename temp_result::result_type::column_dimention,
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
			typename temp_result::result_type::row_dimention,
			typename temp_result::result_type::column_dimention,
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
			typename temp_result::result_type::row_dimention,
			typename temp_result::result_type::column_dimention> final_result;

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
		static_assert(M1::column_dimention::rank == M2::row_dimention::rank, "Row rank of M1 does not match column rank of M2.");

		typedef typename _2_matrix_mul_cost<
			typename M1::row_dimention,
			typename M1::column_dimention,
			typename _matrix_mul_best_cost<M2, M3, _Args...>::result_type::column_dimention> _1_x_args;

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
			typename M1::row_dimention,
			typename M1::column_dimention,
			typename M2::column_dimention,
			typename _matrix_mul_best_cost<M3, _Args...>::result_type::column_dimention> _2_x_args;

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
			typename M1::row_dimention,
			typename M1::column_dimention,
			typename M2::column_dimention,
			typename M3::column_dimention,
			typename _matrix_mul_best_cost<_Args...>::result_type::column_dimention> _3_x_args;

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
		static_assert(M1::column_dimention::rank == M2::row_dimention::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimention::rank == M3::row_dimention::rank, "Row rank of M2 does not match column rank of M3.");

		typedef typename _3_matix_mul_best_cost<
			typename M1::row_dimention,
			typename M1::column_dimention,
			typename M2::column_dimention,
			typename M3::column_dimention>::type type;

		typedef typename type::result_type result_type;

		static const size_t cost = type::cost;
	};

	template <class M1, class M2, class M3, class M4>
	struct _matrix_mul_best_cost<M1, M2, M3, M4>
	{
		static_assert(M1::column_dimention::rank == M2::row_dimention::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimention::rank == M3::row_dimention::rank, "Row rank of M2 does not match column rank of M3.");
		static_assert(M3::column_dimention::rank == M4::row_dimention::rank, "Row rank of M3 does not match column rank of M4.");

		typedef typename _4_matix_mul_best_cost<
			typename M1::row_dimention,
			typename M1::column_dimention,
			typename M2::column_dimention,
			typename M3::column_dimention,
			typename M4::column_dimention>::type type;

		typedef typename type::result_type result_type;

		static const size_t cost = type::cost;
	};

	template <class M1, class M2, class M3, class M4, class M5>
	struct _matrix_mul_best_cost<M1, M2, M3, M4, M5>
	{
		static_assert(M1::column_dimention::rank == M2::row_dimention::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimention::rank == M3::row_dimention::rank, "Row rank of M2 does not match column rank of M3.");
		static_assert(M3::column_dimention::rank == M4::row_dimention::rank, "Row rank of M3 does not match column rank of M4.");
		static_assert(M4::column_dimention::rank == M5::row_dimention::rank, "Row rank of M4 does not match column rank of M5.");

		typedef typename _5_matix_mul_best_cost<
			typename M1::row_dimention,
			typename M1::column_dimention,
			typename M2::column_dimention,
			typename M3::column_dimention,
			typename M4::column_dimention,
			typename M5::column_dimention>::type type;

		typedef typename type::result_type result_type;

		static const size_t cost = type::cost;
	};

	template <class M1, class M2>
	matrix<
		typename M1::row_dimention,
		typename M2::column_dimention>
	multiply(
		const M1& m1,
		const M2& m2)
	{
		static_assert(M1::column_dimention::rank == M2::row_dimention::rank, "Row rank of M1 does not match column rank of M2.");

		return m1 * m2;
	}

	template <class M1, class M2, class M3>
	matrix<
		typename M1::row_dimention,
		typename M3::column_dimention>
	multiply(
		const M1& m1,
		const M2& m2,
		const M3& m3)
	{
		static_assert(M1::column_dimention::rank == M2::row_dimention::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimention::rank == M3::row_dimention::rank, "Row rank of M2 does not match column rank of M3.");

		return _3_matix_mul_best_cost<
			typename M1::row_dimention,
			typename M1::column_dimention,
			typename M2::column_dimention,
			typename M3::column_dimention>::type::multiply(m1, m2, m3);
	}

	// Variadic template for chain multiplication of 4 or more matrices.
	template <class M1, class M2, class M3, class... _Args>
	matrix<
		typename M1::row_dimention,
		typename last<_Args...>::type::column_dimention>
	multiply(
		const M1& m1,
		const M2& m2,
		const M3& m3,
		const _Args&... args)
	{
		static_assert(M1::column_dimention::rank == M2::row_dimention::rank, "Row rank of M1 does not match column rank of M2.");
		static_assert(M2::column_dimention::rank == M3::row_dimention::rank, "Row rank of M2 does not match column rank of M3.");

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