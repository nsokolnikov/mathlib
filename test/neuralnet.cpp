#include "stdafx.h"
#include <unittest.h>
#include <neuralnet.h>

void test_neural_network()
{
	scenario sc("Test for machine_learning::neural_network");

	machine_learning::neural_network<D3, D1> l;
	machine_learning::neural_network<D3, D6, D5, D1> n;

	algebra::vector<D3> positive{ 0.5, 10.0, 0.5 };
	algebra::vector<D3> negative{ 1.0, 1.0, 1.0 };
	algebra::vector<D1> positive_target{ 1.0 };
	algebra::vector<D1> negative_target{ 0.0 };

	const double rate = 0.05;

	test::verbose("Training the neural network");

	for (int i = 0; i < 4000; ++i)
	{
		l.train(positive, positive_target, rate);
		n.train(positive, positive_target, rate);

		l.train(negative, negative_target, rate);
		n.train(negative, negative_target, rate);
	}

	test::verbose("Verifying network on training input");

	auto out = l.process(positive);
	test::assert(out(0) > 0.9, "One layer: Test on positive input below expected confidence level");

	auto nout = n.process(positive);
	test::assert(nout(0) > 0.9, "Three layers: Test on positive input below expected confidence level");

	out = l.process(negative);
	test::assert(out(0) < 0.1, "One layer: Test on negative input above expected confidence level");

	nout = n.process(negative);
	test::assert(nout(0) < 0.1, "Three layers: Test on negative input above expected confidence level");

	sc.pass();
}

void test_composite_networks()
{
	{
		scenario sc("Test for machine_learning::convolution_2d_network");

		algebra::vector<D4> input({ 0.0, 1.0, -1.0, 1.0 });

		auto convolution = machine_learning::convolution_2d<D2, D2>(
			algebra::matrix<D2, D1>({ 0.5, -0.5 }),
			machine_learning::convolution_2d<D2, D2>(
				algebra::matrix<D1, D2>({ 0.5, -0.5 }),
				machine_learning::neural_network<D4, D5, D1>()));

		auto copy = convolution;
		test::assert(convolution.process(input) == copy.process(input), "Different processing result after network copy constructor.");

		algebra::vector<D1> target = { 1.0 };
		convolution.train(input, target, 0.01);
		test::assert(convolution.process(input) != copy.process(input), "Identical processing result after network training.");

		copy = convolution;
		test::assert(convolution.process(input) == copy.process(input), "Different processing result after network copy operator.");

		sc.pass();
	}

	{
		scenario sc("Test for machine_learning::sampling_2d_network");

		algebra::vector<D4> input({ 0.0, 1.0, -1.0, 1.0 });

		auto sampling =
			machine_learning::sampling_2d<algebra::vector<D4>, D2, D2, D1, D2>(
			algebra::matrix<D1, D2>({ 0.5, 0.5 }),
			machine_learning::neural_network<D2, D3, D1>());

		auto copy = sampling;
		test::assert(sampling.process(input) == copy.process(input), "Different processing result after network copy constructor.");

		algebra::vector<D1> target = { 1.0 };
		sampling.train(input, target, 0.01);
		test::assert(sampling.process(input) != copy.process(input), "Identical processing result after network training.");

		copy = sampling;
		test::assert(sampling.process(input) == copy.process(input), "Different processing result after network copy operator.");

		sc.pass();
	}

	{
		scenario sc("Test for machine_learning::network_ensemble");

		algebra::vector<D4> input({ 0.0, 1.0, -1.0, 1.0 });

		auto ensemble = machine_learning::ensemble(
			machine_learning::network<D4, D1>(),
			machine_learning::convolution_2d<D2, D2>(
				algebra::matrix<D2, D1>({ 0.5, -0.5 }),
				machine_learning::convolution_2d<D2, D2>(
					algebra::matrix<D1, D2>({ 0.5, -0.5 }),
					machine_learning::neural_network<D4, D5, D1>())),
			machine_learning::sampling_2d<algebra::vector<D4>, D2, D2, D1, D2>(
				algebra::matrix<D1, D2>({ 0.5, 0.5 }),
				machine_learning::neural_network<D2, D3, D1>()));

		auto copy = ensemble;
		test::assert(ensemble.process(input) == copy.process(input), "Different processing result after network copy constructor.");

		algebra::vector<D1> target = { 1.0 };
		ensemble.train(input, target, 0.01);
		test::assert(ensemble.process(input) != copy.process(input), "Identical processing result after network training.");

		copy = ensemble;
		test::assert(ensemble.process(input) == copy.process(input), "Different processing result after network copy operator.");

		sc.pass();
	}
}
