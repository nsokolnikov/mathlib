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
			: m_weights(weights::random(-0.5, 0.5)), m_net_input(), m_output(), m_delta(), m_bias(output::random(-0.5, 0.5))
		{}

		const output& process(
			const input& data,
			bool training = false)
		{
			m_net_input = m_weights * data;

			// Bias transforms input vector from (x1, ..., xN) into (x1, ..., xN, 1.0),
			// but to avoid input vector reallocs weights for bias column are kept separately
			// from the weight matrix, and bias column is handled individually.
			// Since input for bias column is always 1.0, then weight(row, bias) * 1.0
			// is the same as weight(row, bias), so it is sufficient to simply add bias weight
			// to the weighted sum of input.
			m_net_input += m_bias;

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
			// Update main weight table first. It is possible to use std::transform
			// with column iterators, however the resulting code is less efficient
			// compared to this version.
			for (size_t col = 0; col < weights::column_rank; ++col)
			{
				const value_type factor = data(col) * rate;
				for (size_t row = 0; row < weights::row_rank; ++row)
				{
					m_weights(row, col) += (m_delta(row) + regularization * m_weights(row, col)) * factor;
				}
			}

			// And now update weights for bias column. This time std::transform is used
			// to take advantage of vector iterators and memory prefetcher.
			std::transform(
				m_bias.cbegin(), m_bias.cend(),
				m_delta.cbegin(),
				m_bias.begin(),
				[regularization, rate](const value_type& bias, const value_type& delta)
				{
					return bias + (delta + regularization * bias) * rate;
				});
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
		output m_bias;
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

	// Utility template for a view projection of a vector into a 2D grid of values.
	// Used to apply sampling and convolution on input vectors that represent 2D data input.
	template <
		class X,
		class Y,
		class _Input>
	class projection_2d
	{
	public:
		typedef typename X x_dimension;
		typedef typename Y y_dimension;
		typedef projection_2d<x_dimension, y_dimension, _Input> _Self;
		static const size_t x_rank = x_dimension::rank;
		static const size_t y_rank = y_dimension::rank;

		static_assert(std::is_base_of<algebra::dimension<x_rank>, X>::value, "Type parameter X must be a dimension.");
		static_assert(std::is_base_of<algebra::dimension<y_rank>, Y>::value, "Type parameter Y must be a dimension.");

		typedef typename _Input input;
		typedef typename input::value_type value_type;

		static_assert(input::rank == _Self::x_rank * _Self::y_rank, "Input vector dimension does not match projectection dimensions.");

		static value_type get(
			const input& vect,
			size_t x,
			size_t y,
			const value_type defaultValue)
		{
			if (x >= _Self::x_rank || y >= _Self::y_rank)
				return defaultValue;

			return vect(y * _Self::x_rank + x);
		}

		static void set(
			input& vect,
			size_t x,
			size_t y,
			const value_type val)
		{
			if (x < _Self::x_rank && y < _Self::y_rank)
			{
				vect(y * _Self::x_rank + x) = val;
			}
		}
	};

	// Implementation of a neural net that applies a 2D convolution with a given core
	// to the input data before passing it to the inner network.
	template <
		class _Projection,
		class _Core,
		class _Network>
	class convolution_2d_network
	{
	public:
		typedef typename convolution_2d_network<_Projection, _Core, _Network> _Self;
		typedef typename _Network network;
		typedef typename _Core core;

		typedef typename network::input input;
		typedef typename network::output output;
		typedef typename network::value_type value_type;

		typedef typename _Projection::input projection_input;

		static_assert(std::is_base_of<input, projection_input>::value, "Network input type does not match projection input type.");

		convolution_2d_network(const core& core)
			: m_network(), m_core(core)
		{
		}

		convolution_2d_network(const core& core, const network& net)
			: m_network(net), m_core(core)
		{
		}

		convolution_2d_network(const _Self& other)
			: m_network(other.m_network), m_core(other.m_core)
		{
		}

		convolution_2d_network(_Self&& other)
			: m_network(std::move(other.m_network)), m_core(std::move(other.m_core))
		{
		}

		_Self& operator=(const _Self& other)
		{
			this->m_network = other.m_network;
			this->m_core = other.m_core;
			
			return *this;
		}

		const output& process(
			const input& data)
		{
			input convolution = apply_convolution(data);

			return m_network.process(convolution);
		}

		void train(const input& data,
			const output& target,
			value_type rate,
			value_type regularization = 0.000001)
		{
			input convolution = apply_convolution(data);

			m_network.train(convolution, target, rate, regularization);
		}

	private:
		input apply_convolution(const input& data) const
		{
			input result;

			for (size_t y = 0; y < _Projection::y_rank; ++y)
			{
				for (size_t x = 0; x < _Projection::x_rank; ++x)
				{
					value_type value = 0.0;

					for (size_t row = 0; row < _Core::row_rank; ++row)
					{
						for (size_t column = 0; column < _Core::column_rank; ++column)
						{
							value += _Projection::get(data, x + column, y + row, 0.0) * m_core(row, column);
						}
					}

					_Projection::set(result, x, y, value);
				}
			}

			return result;
		}

	private:
		network m_network;
		core m_core;
	};

	// Implementation of a neural net that applies a 2D sampling with a given core weights
	// to the input data before passing it to the inner network.
	template <
		class _InputProjection,
		class _OutputProjection,
		class _Core,
		class _Network>
	class sampling_2d_network
	{
	public:
		typedef typename sampling_2d_network<_InputProjection, _OutputProjection, _Core, _Network> _Self;

		typedef typename _Network network;
		typedef typename _Core core;

		typedef typename _InputProjection::input input;
		typedef typename network::output output;
		typedef typename network::value_type value_type;

		typedef typename network::input network_input;
		typedef typename _InputProjection::input input_projection_input;
		typedef typename _OutputProjection::input output_projection_input;
		typedef typename _OutputProjection::input output_projection_output;

		static_assert(std::is_base_of<input, input_projection_input>::value, "Network input type does not match output projection input type.");
		static_assert(std::is_base_of<network_input, output_projection_output>::value, "Inner network input type does not match output projection output type.");

		static_assert(_OutputProjection::x_rank * _Core::column_rank < _InputProjection::x_rank + _Core::column_rank, "Sampling on horizontal dimension is invalid.");
		static_assert(_OutputProjection::y_rank * _Core::row_rank < _InputProjection::y_rank + _Core::row_rank, "Sampling on horizontal dimension is invalid.");

		sampling_2d_network(const core& core)
			: m_network(), m_core(core)
		{
		}

		sampling_2d_network(const core& core, const network& n)
			: m_network(n), m_core(core)
		{
		}

		sampling_2d_network(const _Self& other)
			: m_network(other.m_network), m_core(other.m_core)
		{
		}

		sampling_2d_network(_Self&& other)
			: m_network(std::move(other.m_network)), m_core(std::move(other.m_core))
		{
		}

		_Self& operator=(const _Self& other)
		{
			this->m_network = other.m_network;
			this->m_core = other.m_core;

			return *this;
		}

		const output& process(
			const input& data)
		{
			network_input sampling = apply_sampling(data);

			return m_network.process(sampling);
		}

		void train(const input& data,
			const output& target,
			value_type rate,
			value_type regularization = 0.000001)
		{
			network_input sampling = apply_sampling(data);

			m_network.train(sampling, target, rate, regularization);
		}

	private:
		network_input apply_sampling(const input& data) const
		{
			network_input result;

			for (size_t y = 0; y < _OutputProjection::y_rank; ++y)
			{
				for (size_t x = 0; x < _OutputProjection::x_rank; ++x)
				{
					value_type value = 0.0;
					size_t base_x = x * _Core::column_rank;
					size_t base_y = y * _Core::row_rank;

					for (size_t row = 0; row < _Core::row_rank; ++row)
					{
						for (size_t column = 0; column < _Core::column_rank; ++column)
						{
							value += _InputProjection::get(data, base_x + column, base_y + row, 0.0) * m_core(row, column);
						}
					}

					_OutputProjection::set(result, x, y, value);
				}
			}

			return result;
		}

	private:
		network m_network;
		core m_core;
	};

	template <class _N1, class _N2, class... _Args>
	class _network_ensemble_impl : public _network_ensemble_impl<_N2, _Args...>
	{
	public:
		static_assert(std::is_same<typename _N1::input, typename _N2::input>::value, "Network input types do not match.");
		static_assert(std::is_same<typename _N1::output, typename _N2::output>::value, "Network output types do not match.");
		static_assert(std::is_same<typename _N1::value_type, typename _N2::value_type>::value, "Network value types do not match.");

		typedef typename _network_ensemble_impl<_N1, _N2, _Args...> _Self;
		typedef typename _network_ensemble_impl<_N2, _Args...> _Base;
		typedef typename _N1::input input;
		typedef typename _Base::output output;
		typedef typename _Base::value_type value_type;
	
		_network_ensemble_impl(const _N1& n1, const _N2& n2, const _Args&... args)
			: _Base(n2, args...), m_network(n1)
		{
		}

		_network_ensemble_impl(const _Self& other)
			: _Base(other), m_network(other.m_network)
		{
		}

		_Self& operator=(const _Self& other)
		{
			this->m_network = other.m_network;
			*((_Base*)this) = (const _Base&)other;
			
			return *this;
		}

		void process(
			const input& data,
			output& result)
		{
			_Base::process(data, result);
			const output& result2 = m_network.process(data);

			std::transform(
				result.cbegin(), result.cend(),
				result2.cbegin(),
				result.begin(),
				[](const value_type& r1, const value_type& r2)
			{
				return std::max(r1, r2);
			});
		}

		void train(const input& data,
			const output& target,
			value_type rate,
			value_type regularization = 0.000001)
		{
			_Base::train(data, target, rate, regularization);
			m_network.train(data, target, rate, regularization);
		}

	private:
		_N1 m_network;
	};

	template <class _N1, class _N2>
	class _network_ensemble_impl<_N1, _N2>
	{
	public:
		static_assert(std::is_same<typename _N1::input, typename _N2::input>::value, "Network input types do not match.");
		static_assert(std::is_same<typename _N1::output, typename _N2::output>::value, "Network output types do not match.");
		static_assert(std::is_same<typename _N1::value_type, typename _N2::value_type>::value, "Network value types do not match.");

		typedef typename _network_ensemble_impl<_N1, _N2> _Self;
		typedef typename _N1::input input;
		typedef typename _N1::output output;
		typedef typename _N1::value_type value_type;

		_network_ensemble_impl(const _N1& n1, const _N2& n2)
			: m_network1(n1), m_network2(n2)
		{
		}

		_network_ensemble_impl(const _Self& other)
			: m_network1(other.m_network1), m_network2(other.m_network2)
		{
		}

		_Self& operator=(const _Self& other)
		{
			m_network1 = other.m_network1;
			m_network2 = other.m_network2;
			return *this;
		}

		void process(
			const input& data,
			output& result)
		{
			const output& result1 = m_network1.process(data);
			const output& result2 = m_network2.process(data);

			std::transform(
				result1.cbegin(), result1.cend(),
				result2.cbegin(),
				result.begin(),
				[](const value_type& r1, const value_type& r2)
			{
				return std::max(r1, r2);
			});
		}

		void train(const input& data,
			const output& target,
			value_type rate,
			value_type regularization = 0.000001)
		{
			m_network1.train(data, target, rate, regularization);
			m_network2.train(data, target, rate, regularization);
		}

	private:
		_N1 m_network1;
		_N2 m_network2;
	};

	// Variadic template for a neural network ensemble that computes the output
	// by applying max-pooling to the results of all networks in the ensemble.
	template <class _N1, class _N2, class... _Args>
	class network_ensemble : protected _network_ensemble_impl<_N1, _N2, _Args...>
	{
	public:
		typedef typename network_ensemble<_N1, _N2, _Args...> _Self;
		typedef typename _network_ensemble_impl<_N1, _N2, _Args...> _Base;
		typedef typename _Base::input input;
		typedef typename _Base::output output;
		typedef typename _Base::value_type value_type;

		network_ensemble(const _N1& n1, const _N2& n2, const _Args&... args)
			: _Base(n1, n2, args...), m_output()
		{
		}

		network_ensemble(const _Self& other)
			: _Base(other), m_output(other.m_output)
		{
		}

		_Self& operator=(const _Self& other)
		{
			*((_Base*)this) = (const _Base&)other;
			m_output = other.m_output;
			return *this;
		}

		const output& process(
			const input& data)
		{
			_Base::process(data, m_output);

			return m_output;
		}

		void train(const input& data,
			const output& target,
			value_type rate,
			value_type regularization = 0.000001)
		{
			_Base::train(data, target, rate, regularization);
		}

	private:
		output m_output;
	};

	template <class... _Layers>
	neural_network<_Layers...> network()
	{
		return neural_network<_Layers...>();
	}

	template <
		class _X, 
		class _Y,
		class _Core,
		class _Network>
	convolution_2d_network<projection_2d<_X, _Y, typename _Network::input>, _Core, _Network> convolution_2d(
		const _Core& core,
		const _Network& network)
	{
		return convolution_2d_network<projection_2d<_X, _Y, typename _Network::input>, _Core, _Network>(core, network);
	}

	template <
		class _Input,
		class _XIn, 
		class _YIn,
		class _XOut,
		class _YOut,
		class _Core,
		class _Network>
	sampling_2d_network<projection_2d<_XIn, _YIn, _Input>, projection_2d<_XOut, _YOut, typename _Network::input>, _Core, _Network> sampling_2d(
		const _Core& core,
		const _Network& network)
	{
		return sampling_2d_network<projection_2d<_XIn, _YIn, _Input>, projection_2d<_XOut, _YOut, typename _Network::input>, _Core, _Network>(core, network);
	}

	template <class... _Networks>
	network_ensemble<typename std::_Unrefwrap<_Networks>::type...> ensemble(
		_Networks&&... args)
	{
		typedef network_ensemble<typename std::_Unrefwrap<_Networks>::type...> _Ntype;
		return (_Ntype(std::forward<_Networks>(args)...));
	}
}
