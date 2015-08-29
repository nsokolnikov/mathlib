// MathLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <unittest.h>
#include <neuralnet.h>

struct D14 : public algebra::dimension<14> {};
struct D21 : public algebra::dimension<21> {};

double normalize(const double in, const double max)
{
	return machine_learning::_LayerBase::activation(in / max);
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

	machine_learning::neural_network<D8, D7, D3> n;

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

	machine_learning::neural_network<D3, D1> l;
	algebra::vector<D3> positive{ 0.5, 10.0, 0.5 };
	algebra::vector<D3> negative{ 1.0, 1.0, 1.0 };
	algebra::vector<D1> positive_target{ 1.0 };
	algebra::vector<D1> negative_target{ 0.0 };

	machine_learning::neural_network<D3, D5, D1> n;

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
