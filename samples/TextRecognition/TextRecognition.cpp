#include "stdafx.h"
#include <neuralnet.h>
#include "mnist.h"
struct D4 : public algebra::dimension<4> {};

int _tmain(int /*argc*/, _TCHAR* /*argv[]*/)
{
	load_mnist();
	machine_learning::neural_network<D4, D4> n;

	return 0;
}

