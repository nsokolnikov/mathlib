#include "stdafx.h"
#include <neuralnet.h>
#include "mnist.h"
#include <iostream>

struct D49 : public algebra::dimension<49> {};
struct D10 : public algebra::dimension<10> {};
struct D2 : public algebra::dimension<2> {};
struct D28 : public algebra::dimension<28> {};
struct D14 : public algebra::dimension<14> {};
struct D196 : public algebra::dimension<196> {};

typedef machine_learning::neural_network<D784, D49, D10> oacr_network;
typedef machine_learning::neural_network<D196, D49, D10> oacr_sampled_network;

typedef machine_learning::projection_2d<D14, D14, oacr_sampled_network::input> _14x14;
typedef machine_learning::projection_2d<D28, D28, oacr_network::input> _28x28;

typedef algebra::matrix<D2, D2> convolution_core;
typedef algebra::matrix<D2, D2> sampling_core;

typedef machine_learning::convolution_2d_network<_28x28, convolution_core, oacr_network> convolution_network;
typedef machine_learning::sampling_2d_network<_28x28, _14x14, sampling_core, oacr_sampled_network> sampled_network;

void print_usage()
{
	std::cout << "USAGE:\r\n";
	std::cout << "\r\n";
	std::cout << "TextRecognition.exe data_path\r\n";
	std::cout << "\r\n";
	std::cout << "    data_path  Path to the MNIST training data set.\r\n";
}

const oacr_network::output& get_target(
	int digit)
{
	static std::vector<oacr_network::output> targets{
		{ 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 },
	};

	if (digit < 0 || (int)targets.size() < digit)
		throw std::invalid_argument("Input digit is out of range");

	return targets[digit];
}

const int get_result(
	oacr_network::output& output,
	double& confidence)
{
	double max = output(0);
	size_t maxIndex = 0;
	for (size_t i = 1; i < oacr_network::output::rank; ++i)
	{
		if (max < output(i))
		{
			maxIndex = i;
			max = output(i);
		}
	}

	confidence = max;
	return (int)maxIndex;
}

std::vector<double> get_learning_rates(
	double rate,
	const double factor,
	const size_t levels)
{
	std::vector<double> result;
	result.reserve(levels);

	for (size_t i = 0; i < levels; ++i)
	{
		result.push_back(rate);
		rate *= factor;
	}

	return result;
}

template <class _Network>
void test_success_rate(
	_Network& network,
	const mnist_data& training,
	std::string prefix)
{
	size_t errors = 0;
	for (auto digit : training)
	{
		auto result = network.process(digit.second);
		double confidence;
		int recognized = get_result(result, confidence);

		if (digit.first != recognized)
		{
			++errors;
		}
	}

	std::cout << prefix.c_str() 
		<< " success rate: " << ((double)(training.size() - errors) / (double)training.size())
		<< " error rate: " << ((double)(errors) / (double)training.size())
		<< "\r\n";
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		print_usage();
		return 1;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> distr(0, 1);

	mnist_data full = load_mnist(std::wstring(argv[1]));
	mnist_data training;
	mnist_data test;

	while (full.size() > 0)
	{
		int digitId = full.back().first;

		size_t segment = full.size();
		while (segment > 0 && digitId == full[segment - 1].first)
		{
			--segment;
		}

		while (full.size() > segment)
		{
			size_t segmentSize = full.size() - segment;
			size_t nextIndex = segment + (size_t)(((double)segmentSize) * distr(gen));

			if (segmentSize > 1)
			{
				std::swap(full[nextIndex], full[full.size() - 1]);
			}

			if (full.size() % 10 < 2)
			{
				test.push_back(full.back());
			}
			else
			{
				training.push_back(full.back());
			}

			full.pop_back();
		}
	}

	auto network = machine_learning::ensemble(
		oacr_network(),
		convolution_network({ -1.0, 1.0, 0.0, 0.0 }, oacr_network()),
		convolution_network({ -1.0, 0.0, 1.0, 0.0 }, oacr_network()),
		sampled_network({ 0.25, 0.25, 0.25, 0.25 }, oacr_sampled_network()));

	test_success_rate(network, training, "Untrained");

	std::vector<double> rates = get_learning_rates(1.5, 0.7, 10);

	std::vector<const mnist_digit*> input;

	// Training
	for (auto rate : rates)
	{
		std::cout << "Training with rate " << rate << "\r\n";

		for (int i = 0; i < 3; ++i)
		{
			input.resize(training.size());
			std::transform(
				training.cbegin(), training.cend(),
				input.begin(),
				[](const mnist_digit& digit) { return std::addressof(digit); });

			while (input.size() > 0)
			{
				size_t nextIndex = (size_t)(((double)input.size()) * distr(gen));
				const mnist_digit* digit = input[nextIndex];

				if (input.size() > 1)
				{
					std::swap(input[nextIndex], input[input.size() - 1]);
				}
				input.pop_back();

				network.train(digit->second, get_target(digit->first), rate);
			}
		}

		test_success_rate(network, training, "Training set");
		test_success_rate(network, test, "Test set");
	}

	// Recognition
	size_t errors = 0;
	size_t maxTests = 1000;
	for (size_t i = 0; i < maxTests; ++i)
	{
		auto digit = test[(size_t)(((double)test.size()) * distr(gen))];

		auto result = network.process(digit.second);

		double confidence;
		int recognized = get_result(result, confidence);
		std::cout << "Actual: " << digit.first << "; detected: " << recognized << "; confidence: " << confidence;
		if (digit.first != recognized)
		{
			std::cout << " <-- Error";
			++errors;
		}

		std::cout << "\r\n";
	}

	std::cout
		<< "Random sampling success rate: " << ((double)(maxTests - errors) / (double)maxTests)
		<< " error rate: " << ((double)(errors) / (double)maxTests)
		<< "\r\n";

	full = load_mnist(std::wstring(argv[1]));

	test_success_rate(network, full, "Full");

	return 0;
}

