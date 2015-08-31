// Sample application of neural network that detects deviations
// from a normal activity. The network is trained on a simulated
// input that imitates variable activity for different days of a week.
// The network is trained to recognize and report low, normal,
// or high activity for that day, assuming that +-20% deviation 
// from the ideal target is still a normal activity.

#include "stdafx.h"
#include <neuralnet.h>
#include <iostream>
#include "mnist.h"

struct D3 : public algebra::dimension<3> {};
struct D7 : public algebra::dimension<7> {};
struct D8 : public algebra::dimension<8> {};

double normalize(
	const double in,
	const double max)
{
	return machine_learning::_LayerBase::activation(in / max);
}

algebra::vector<D8> make_input(
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

int _tmain(int /*argc*/, _TCHAR* /*argv[]*/)
{


	load_mnist();
	// Initialize ideal load targets. The set imitates weekly activity
	// assuming 5 business days and 2 hollidays.
	std::vector<double> traning{ 3500.0, 50000.0, 60000.0, 62000.0, 58000.0, 55000.0, 4000.0 };
	const double max = 65000;

	machine_learning::neural_network<D8, D7, D3> n;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> distr(0, 1);
	std::normal_distribution<double> gauss(0, 0.15);

	algebra::vector<D3> targetNormal{ 1.0, 0.0, 0.0 };
	algebra::vector<D3> targetLow{ 0.0, 1.0, 0.0 };
	algebra::vector<D3> targetHigh{ 0.0, 0.0, 1.0 };

	// Traning parameters. Rate determines the speed and accuracy of
	// training, and maxIterations determines the number of training
	// iterations. Each iteration consists of training data for one week
	// feeding input in random order.
	const double rate = 0.6;
	const int maxIterations = 400000;

	std::cout << "Training network...\r\n";

	for (int i = 0; i < maxIterations; ++i)
	{
		if (i % (maxIterations / 10) == (maxIterations / 10) - 1)
		{
			std::cout << "Completed " << i << " out of " << maxIterations << " iterations.\r\n";
		}

		std::vector<size_t> input{ 0, 1, 2, 3, 4, 5, 6 };

		// We use stochastic method of training, and the training set is
		// generated from random deviations from ideal input. The load that
		// is more than 20% lower than the ideal input is considered low,
		// and the load that is 20% higher than the ideal input is considered high.
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

	// After the network is trained, test the accuracy of the detection
	// by creating a simulated input for the specific input category.
	//
	// For normal, low, and high test input we use uniform distribution,
	// and for random input we use gaussian distribution from the ideal
	// target for that day to simulate input values.
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

	return 0;
}
