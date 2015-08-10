// MathLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <unittest.h>
#include <matrix.h>

template <
	class _InIt1,
	class _InIt2,
	class _InIt3,
	class _OutIt,
	class _Fn3> inline
_OutIt transform(
	_InIt1 _First1,
	_InIt1 _Last1,
	_InIt2 _First2,
	_InIt3 _First3,
	_OutIt _Dest,
	_Fn3 _Func)
{	// transform [_First1, _Last1) and [_First2, ...) and [_First3, ...) with _Func
	for (; _First1 != _Last1; ++_First1, ++_First2, ++_First3, ++_Dest)
		*_Dest = _Func(*_First1, *_First2, *_First3);
	return (_Dest);
}

class _LayerBase
{
public:
	static const double activation(const double& x)
	{
		// Logistic function: F(x) = 1 / (1 + e^-x) == e^x / (1 + e^x)
		const double eX = std::exp(x);
		return eX / (1 + eX);
	}

	static const double activation_derivative(const double& x)
	{
		// Derivative of the logistic function is F(x) * (1 - F(x))
		const double logistic = _LayerBase::activation(x);
		return logistic * (1.0 - logistic);
	}
};

template <class _Input, class _Output>
class layer : public _LayerBase
{
public:
	typedef typename layer<_Input, _Output> _Self;
	typedef typename algebra::vector<_Input> input;
	typedef typename algebra::vector<_Output> output;
	typedef typename algebra::matrix<_Output, _Input> weights;

	typedef typename weights::value_type value_type;

	layer()
		: m_weights(weights::random(-0.5, 0.5)), m_net_input(), m_output(), m_delta()
	{}

	const output& process(
		const input& data,
		bool training = false)
	{
		this->m_net_input = m_weights * data;

		std::transform(
			m_net_input.cbegin(), m_net_input.cend(),
			m_output.begin(),
			&_Self::activation);

		if (false == training)
		{
			m_net_input.clear();
			m_delta.clear();
		}

		return m_output;
	}

	void compute_output_delta(
		const output& target)
	{
		// Compute cost function gradient for the output layer.
		transform(
			target.begin(), target.end(),
			m_output.cbegin(),
			m_net_input.cbegin(),
			m_delta.begin(),
			[](const value_type& t, const value_type& o, const value_type& net)
		{
			return (o - t) * _Self::activation_derivative(net);
		});
	}

	template <class _NextOutput>
	void compute_inner_delta(
		const typename layer<_Output, _NextOutput>& nextLayer)
	{
		// Compute cost function gradient for hidden layer using 
		// weights and gradient from the next layer, which are
		// expected to be computed at this point.
		typedef typename typename layer<_Output, _NextOutput> _Next;

		for (size_t i = 0; i < output::rank; ++i)
		{
			value_type sum = 0.0;

			auto dN = nextLayer.delta();
			auto wN = nextLayer.weight();
			for (size_t j = 0; j < _Next::output::rank; ++j)
			{
				sum += dN(j) * wN(j, i);
			}

			m_delta(i) = sum * _Self::activation_derivative(m_net_input(i));
		}
	}

	void update_weights(
		const input& data,
		value_type rate)
	{
		// Weights are updated in the direction oposite to gradient.
		// Make sure that correct learning rate value is used.
		rate = -std::abs(rate);

		for (size_t row = 0; row < weights::row_rank; ++row)
		{
			for (size_t col = 0; col < weights::column_rank; ++col)
			{
				m_weights(row, col) += m_delta(row) * data(col) * rate;
			}
		}
	}

	const output& delta() const
	{
		return m_delta;
	}

	const weights& weight() const
	{
		return m_weights;
	}

	const output& last_output() const
	{
		return m_output;
	}

private:
	weights m_weights;
	output m_net_input;
	output m_output;
	output m_delta;
};

template <class _L1, class _L2, class... _Args>
class _network_impl : public _network_impl<_L2, _Args...>
{
public:
	typedef typename _network_impl<_L1, _L2, _Args...> _Self;
	typedef typename _network_impl<_L2, _Args...> _Base;
	typedef typename layer<_L1, _L2> this_layer;
	typedef typename this_layer::input input;
	typedef typename _Base::output output;
	typedef typename _Base::value_type value_type;

	const output& process(
		const input& data,
		bool training)
	{
		// Feed forward results from this layer into the next layer
		return _Base::process(
			m_hidden.process(data, training),
			training);
	}

	void compute_delta(const output& target)
	{
		// For backpropagation, compute cost gradient for the
		// next layer first, and then compute cost gradient
		// for the current hidden layer.
		_Base::compute_delta(target);
		m_hidden.compute_inner_delta(_Base::get_layer());
	}

	void update_weights(
		const input& data,
		value_type rate)
	{
		// Finally, update weights for this and next layers.
		// Since the members of base class are allocated in memory
		// before the members of the derived class, update weights
		// from the base class first for a slight perf benefit.
		_Base::update_weights(m_hidden.last_output(), rate);
		m_hidden.update_weights(data, rate);
	}

	const this_layer& get_layer() const
	{
		return m_hidden;
	}

private:
	this_layer m_hidden;
};

// Explicit instantiation for the final (output) layer.
// This implementation acts as exit point from recursion.
template <class _L1, class _L2>
class _network_impl<_L1, _L2>
{
public:
	typedef typename layer<_L1, _L2> this_layer;
	typedef typename this_layer::input input;
	typedef typename this_layer::output output;
	typedef typename this_layer::value_type value_type;

	const output& process(
		const input& data,
		bool training)
	{
		return m_output.process(data, training);
	}

	void compute_delta(const output& target)
	{
		m_output.compute_output_delta(target);
	}

	void update_weights(const input& data,
		value_type rate)
	{
		m_output.update_weights(data, rate);
	}

	const this_layer& get_layer() const
	{
		return m_output;
	}

private:
	this_layer m_output;
};

// Variadic template for a feed-forward neural network
// that is trained using backpropagation algorithm.
template <class _L1, class _L2, class... _Args>
class network : protected _network_impl<_L1, _L2, _Args...>
{
public:
	typedef typename _network_impl<_L1, _L2, _Args...> _Base;
	typedef typename _Base::input input;
	typedef typename _Base::output output;
	typedef typename _Base::value_type value_type;

	const output& process(
		const input& data)
	{
		return _Base::process(data, false);
	}

	void train(const input& data,
		const output& target,
		value_type rate)
	{
		// First, recursively process input through all layers.
		// Training mode enables caching of important intermediate
		// values which are used late.
		_Base::process(data, true);

		// Then recursively compute gradient and propagate it
		// in backward direction.
		_Base::compute_delta(target);

		// Finally, update the costs of all layers.
		_Base::update_weights(data, rate);
	}
};

struct D14 : public algebra::dimension<14> {};
struct D21 : public algebra::dimension<21> {};

double normalize(const double in, const double max)
{
	return _LayerBase::activation(in / max);
}

algebra::vector<D8>
make_input(
	size_t day,
	const double load,
	const double max)
{
	algebra::vector<D8> result;
	result(day) = 1.0;
	result(7) = normalize(load, max);
	return result;
}

void print_result(
	size_t index,
	int test,
	int ideal,
	algebra::vector<D3>& result)
{
	std::cout << index << ": Input:\t" << test 
		<< ";\tIdeal:\t" << ideal
		<< ";\tNormal:\t" << (result(0) > 0.6)
		<< ";\tLow:\t" << (result(1) > 0.6)
		<< ";\tHigh:\t" << (result(2) > 0.6)
		<< "\r\n";
}

template <class _Network, class _Func>
void simulate_distribution(
	const std::vector<double>& traning,
	_Network n,
	const double max,
	_Func func)
{
	for (size_t i = 0; i < traning.size() * 10; ++i)
	{
		size_t index = (i % traning.size());

		double ideal = traning[index];
		double test = func(ideal);

		auto out = n.process(make_input(index, test, max));

		print_result(index, (int)test, (int)ideal, out);
	}
}

void test_daily_volume()
{
	std::vector<double> traning{ 3500.0, 50000.0, 60000.0, 62000.0, 58000.0, 55000.0, 4000.0 };
	const double max = 65000;

	const double rate = 0.8;

	network<D8, D7, D3> n;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> distr(0, 1);
	std::normal_distribution<double> gauss(0, 0.15);

	algebra::vector<D3> targetNormal{ 1.0, 0.0, 0.0 };
	algebra::vector<D3> targetLow{ 0.0, 1.0, 0.0 };
	algebra::vector<D3> targetHigh{ 0.0, 0.0, 1.0 };

	for (int i = 0; i < 400000; ++i)
	{
		std::vector<size_t> input{ 0, 1, 2, 3, 4, 5, 6 };

		while (input.size() > 0)
		{
			size_t nextIndex = (size_t)(((double)input.size()) * distr(gen));
			size_t index = input[nextIndex];
			if (input.size() > 1)
			{
				std::swap(input[nextIndex], input[input.size() - 1]);
			}
			input.pop_back();

			n.train(make_input(index, traning[index] * (1 + (distr(gen) - 0.5) * 0.2), max), targetNormal, rate);

			n.train(make_input(index, traning[index] * (0.8 * distr(gen)), max), targetLow, rate);

			n.train(make_input(index, traning[index] * (1.2 + 10 * distr(gen)), max), targetHigh, rate);
		}
	}

	std::cout << "Normal input test\r\n";
	simulate_distribution(
		traning,
		n,
		max,
		[&distr, &gen](const double& x)
		{
			return x * (1 + (distr(gen) - 0.5) * 0.05);
		});

	std::cout << "High input test\r\n";
	simulate_distribution(
		traning,
		n,
		max,
		[&distr, &gen](const double& x)
	{
		return x * (1.3 + 20 * distr(gen));
	});

	std::cout << "Low input test\r\n";
	simulate_distribution(
		traning,
		n,
		max,
		[&distr, &gen](const double& x)
	{
		return x * (0.8 * distr(gen));
	});

	std::cout << "Random input test\r\n";
	simulate_distribution(
		traning,
		n,
		max,
		[&gauss, &gen](const double& x)
	{
		return x * std::abs(1.0 + gauss(gen));
	});
}

int _tmain(int /*argc*/, _TCHAR* /*argv[]*/)
{
	test_daily_volume();

	network<D3, D1> l;
	algebra::vector<D3> positive{ 0.5, 10.0, 0.5 };
	algebra::vector<D3> negative{ 1.0, 1.0, 1.0 };
	algebra::vector<D1> positive_target{ 1.0 };
	algebra::vector<D1> negative_target{ 0.0 };

	network<D3, D5, D1> n;

	const double rate = 0.01;

	for (int i = 0; i < 200000; ++i)
	{
		l.train(positive, positive_target, rate);
		n.train(positive, positive_target, rate);

		l.train(negative, negative_target, rate);
		n.train(negative, negative_target, rate);
	}

	auto out = l.process(positive);
	auto nout = n.process(positive);
	out = l.process(negative);
	nout = n.process(negative);

	for (int i = 0; i < 100; ++i)
	{
		algebra::vector<D3> test{ 0.9, 127.0, 4.9 + (1 * i) };
		out = l.process(test);
		nout = n.process(test);
	}

	//test change
	run_tests();

	return 0;
}
