#include "stdafx.h"
#include <neuralnet.h>
#include "mnist.h"
#include <iostream>

struct D25 : public algebra::dimension<25> {};
struct D10 : public algebra::dimension<10> {};

typedef machine_learning::neural_network<D784, D25, D10> oacr_network;

void print_usage()
{
	std::cout << "USAGE:\r\n";
	std::cout << "\r\n";
	std::cout << "TextRecognition.exe data_path\r\n";
	std::cout << "\r\n";
	std::cout << "    data_path  Path to the MNIST training data set.\r\n";
}

const algebra::vector<D10>& get_target(int digit)
{
	static std::vector<algebra::vector<D10>> targets{
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
	algebra::vector<D10>& output,
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

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		print_usage();
		return 1;
	}

	mnist_data training = load_mnist(std::wstring(argv[1]));

	oacr_network network;

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

	std::cout << "Untrained success rate: " << ((double)(training.size() - errors) / (double)training.size()) << "\r\n";

	std::vector<double> rates = get_learning_rates(2.5, 0.6, 10);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> distr(0, 1);

	std::vector<const mnist_digit*> input;

	// Training
	for (auto rate : rates)
	{
		std::cout << "Training with rate " << rate << "\r\n";

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

	// Recognition
	errors = 0;
	size_t maxTests = 1000;
	for (size_t i = 0; i < maxTests; ++i)
	{
		auto digit = training[(size_t)(((double)training.size()) * distr(gen))];

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

	std::cout << "Random sampling success rate: " << ((double)(maxTests - errors) / (double)maxTests) << "\r\n";

	errors = 0;
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

	std::cout << "Full success rate: " << ((double)(training.size() - errors) / (double)training.size()) << "\r\n";

	return 0;
}

