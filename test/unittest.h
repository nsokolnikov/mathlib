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

class test_exception : public std::exception
{
public:
	test_exception(const char* const& message)
		: std::exception(message) {}
};

template <const bool Verbose = false>
class _Test
{
private:
	typedef _Test<Verbose> _Self;

public:
	static bool is_verbose()
	{
		return Verbose;
	}

	// Utility function to log a message from a test scenario.
	//
	// Sample usage:
	//		test::log("This text is always logged.");
	//
	static void log(const char* message)
	{
		std::cout << message << "\r\n";
	}

	// Utility function to log a verbose message from a test scenario.
	//
	// Sample usage:
	//		test::verbose("This text is logged only if verbose mode is enabled.");
	//
	static void verbose(const char* message)
	{
		if (is_verbose())
		{
			log(message);
		}
	}

	// Utility function to assert a test condition.
	//
	// Sample usage:
	//		test::assert(some_check, "My check failed");
	//
	static void assert(bool test, const char* message)
	{
		if (true == test)
			return;

		static std::string log = "ASSERT FAILED: ";
		
		verbose((log + message).c_str());

		throw test_exception(message);
	}

	// Utility function to execute a test function and check for success.
	//
	// Sample usage:
	//		test::check([]() { do_something(); return success; }, "My test scenario failed.");
	//
	template <class _Func>
	static bool check(_Func func, const char* message)
	{
		if (func())
			return true;

		static std::string log = "TEST FAILED: ";
		verbose((log + message).c_str());

		throw test_exception(message);
	}

	// Utility function to execute a test function and check that expected exception is thrown.
	//
	// Sample usage:
	//		test::check_exception<foo>([]() { throw foo() }, "Foo was not thrown.");
	//
	template <class _Ex, class _Func>
	static bool check_exception(_Func func, const char* message)
	{
		return _Self::check([&func]()
			{
				bool pass = false;
				try
				{
					func();
				}
				catch (const _Ex& err)
				{
					static std::string log = "Caught expected exception: ";
					verbose((log + typeid(_Ex).name() + ": " + err.what()).c_str());
					pass = true;
				}

				return pass;
			},
			message);
	}
};

// Default to no verbose logging. Change that to true for debugging.
typedef _Test<false> test;

// Utility struct to automatically check for a test scenario success.
//
// Sample usage:
//		void my_test_scenario()
//		{
//			scenario s("My scenario");
//			...
//			s.pass();
//		}
//
struct scenario
{
	const char* name;
	bool passed;

	scenario(const char* _Name = "Unknown")
		: name(_Name), passed(false)
	{
		static std::string log = "Scenario: ";

		test::log((log + name + ": BEGIN").c_str());
	}

	void pass()
	{
		passed = true;
	}

	~scenario()
	{
		static std::string log = "Scenario: ";

		if (passed)
		{
			test::log((log + name + ": PASS").c_str());
		}
		else
		{
			std::string failure = log + name + ": FAILED";
			test::log(failure.c_str());
			
			throw test_exception(failure.c_str());
		}
	}
};

void test_expressions();

void test_vector();
void test_vector_expressions();

void test_view();

void test_matrix_row_iterators();
void test_matrix_view_row_iterators();

void test_solve();

void run_tests();
