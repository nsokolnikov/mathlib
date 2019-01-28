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
		test_matrices();

		test_view();

		test_matrix_row_iterators();
		test_matrix_column_iterators();
		test_matrix_view_row_iterators();
		test_matrix_view_column_iterators();

		test_vector_iterators();

		test_solve();

		test_neural_network();
		test_composite_networks();

		test_projection();

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
