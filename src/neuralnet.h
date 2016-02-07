#pragma once

#include "matrix.h"

namespace machine_learning
{
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
	class neuron_layer : public _LayerBase
	{
	public:
		typedef typename neuron_layer<_Input, _Output> _Self;
		typedef typename algebra::vector<_Input> input;
		typedef typename algebra::vector<_Output> output;
		typedef typename algebra::matrix<_Output, _Input> weights;

		typedef typename weights::value_type value_type;

		neuron_layer()
			: m_weights(weights::random(-0.5, 0.5)), m_net_input(), m_output(), m_delta()
		{}

		const output& process(
			const input& data,
			bool training = false)
		{
			m_net_input = m_weights * data;

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
			const typename neuron_layer<_Output, _NextOutput>& nextLayer)
		{
			// Compute cost function gradient for hidden layer using 
			// weights and gradient from the next layer, which are
			// expected to be computed at this point.
			typedef typename typename neuron_layer<_Output, _NextOutput> _Next;

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
			value_type rate,
			value_type regularization)
		{
			for (size_t row = 0; row < weights::row_rank; ++row)
			{
				for (size_t col = 0; col < weights::column_rank; ++col)
				{
					m_weights(row, col) += (m_delta(row) + regularization * m_weights(row, col)) * data(col) * rate;
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

	// Implementation of an input or hidden layer in a neural network.
	// Next layer in the network is represented by the base class.
	template <class _L1, class _L2, class... _Args>
	class _network_impl : public _network_impl<_L2, _Args...>
	{
	public:
		typedef typename _network_impl<_L1, _L2, _Args...> _Self;
		typedef typename _network_impl<_L2, _Args...> _Base;
		typedef typename neuron_layer<_L1, _L2> this_layer;
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
			value_type rate,
			value_type regularization)
		{
			// Finally, update weights for this and next layers.
			// Since the members of base class are allocated in memory
			// before the members of the derived class, update weights
			// from the base class first for a slight perf benefit.
			_Base::update_weights(m_hidden.last_output(), rate, regularization);
			m_hidden.update_weights(data, rate, regularization);
		}

		const this_layer& get_layer() const
		{
			return m_hidden;
		}

	private:
		this_layer m_hidden;
	};

	// Explicit instantiation for the final (output) layer in a neural network.
	// This implementation acts as exit point from recursion.
	template <class _L1, class _L2>
	class _network_impl<_L1, _L2>
	{
	public:
		typedef typename neuron_layer<_L1, _L2> this_layer;
		typedef typename this_layer::input input;
		typedef typename this_layer::output output;
		typedef typename this_layer::value_type value_type;

		const output& process(
			const input& data,
			bool training)
		{
			return m_output.process(data, training);
		}

		void compute_delta(
			const output& target)
		{
			m_output.compute_output_delta(target);
		}

		void update_weights(
			const input& data,
			value_type rate,
			value_type regularization)
		{
			m_output.update_weights(data, rate, regularization);
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
	class neural_network : protected _network_impl<_L1, _L2, _Args...>
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
			value_type rate,
			value_type regularization = 0.000001)
		{
			// First, recursively process input through all layers.
			// Training mode enables caching of important intermediate
			// values which are used late.
			_Base::process(data, true);

			// Then recursively compute gradient and propagate it
			// in backward direction.
			_Base::compute_delta(target);

			// Finally, update the costs of all layers.
			// Make sure that correct learning and regularization rate values are used.
			rate = -std::abs(rate);
			regularization = std::abs(regularization);
			_Base::update_weights(data, rate, regularization);
		}
	};
}
