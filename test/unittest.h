#pragma once
#include "declaration.h"

struct D1 : public algebra::dimention<1> {};
struct D2 : public algebra::dimention<2> {};
struct D3 : public algebra::dimention<3> {};
struct D4 : public algebra::dimention<4> {};
struct D5 : public algebra::dimention<5> {};
struct D6 : public algebra::dimention<6> {};
struct D7 : public algebra::dimention<7> {};
struct D8 : public algebra::dimention<8> {};
struct D9 : public algebra::dimention<9> {};
struct D10 : public algebra::dimention<10> {};

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
