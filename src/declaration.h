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

}