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