#pragma once

#include "declaration.h"

struct D1 : public algebra::dimension<1> {};
struct D2 : public algebra::dimension<2> {};
struct D3 : public algebra::dimension<3> {};
struct D4 : public algebra::dimension<4> {};
struct D5 : public algebra::dimension<5> {};
struct D6 : public algebra::dimension<6> {};
struct D7 : public algebra::dimension<7> {};
struct D8 : public algebra::dimension<8> {};
struct D9 : public algebra::dimension<9> {};
struct D10 : public algebra::dimension<10> {};

inline 
void test_assert(bool test, const char* message)
{
	if (!test)
		throw std::exception(message);
}

void test_expressions();

void test_vector();
void test_vector_expressions();

void run_tests();
