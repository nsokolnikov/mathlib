#include "../MathLib/stdafx.h"
#include "unittest.h"

void run_tests()
{
	try
	{
		test_expressions();
		test_vector_expressions();
		test_vector();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << "\r\n";
		throw e;
	}
}
