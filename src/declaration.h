#pragma once

namespace algebra
{
	template <const size_t Rank>
	struct dimension
	{
		static const size_t rank = Rank;

		static_assert(rank > 0, "Dimension cannot be zero.");
	};

	// Utility template to get last template argument from a
	// variadic template argument list.
	template <class T1, class ...T>
	struct last
	{
		typedef typename last<T...>::type type;
	};

	template <class T1>
	struct last<T1>
	{
		typedef T1 type;
	};

	// Utility template to select type with the smallest cost.
	template <class T1, class T2, class... Args>
	struct min_cost
	{
		typedef typename std::conditional<
			(T1::cost < T2::cost),
			typename min_cost<T1, Args...>::type,
			typename min_cost<T2, Args... >::type >::type type;

		static const size_t cost = type::cost;
	};

	template <class T1, class T2>
	struct min_cost<T1, T2>
	{
		typedef typename std::conditional<(T1::cost < T2::cost), T1, T2>::type type;

		static const size_t cost = type::cost;
	};

	template <class _Number>
	struct number_traits
	{
		static bool equals(const _Number& n1, const _Number& n2)
		{
			return (n1 == n2);
		}

		static const _Number zero()
		{
			return 0;
		}
	};

	template<>
	struct number_traits<double>
	{
		static bool equals(
			const double& d1,
			const double& d2)
		{
			const double error = 1.0e-10;

			return std::abs(d1 - d2) < error;
		}

		static const double zero()
		{
			return 0.0;
		}

		static bool is_zero(const double& d) {
			const double error = 1.0e-10;
			return std::abs(d) < error;
		}
	};
}