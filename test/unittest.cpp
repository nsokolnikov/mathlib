#include "stdafx.h"

#include <exception>
#include <iostream>

#include <unittest.h>

void run_tests()
{
	try
	{
		test_expressions();
		test_vector_expressions();
		test_vector();

		test::log("===========================================");
		test::log("All unit tests PASS");
	}
	catch (const std::exception& e)
	{
		test::log("===========================================");
		test::log((std::string("Unexpected exception during unit tests: ") + e.what()).c_str());
		test::log("===========================================");
		test::log("Unit tests FAILED");
	}
}
