#pragma once

namespace ghl
{
/*
* A test case consists of code for execution,
* and the programmer's asserts about the result of the execution.
*
* After the execution,
* the test case is successful iff the code contained in it is fully executed (i.e. not terminated unexpectedly)
* and all assertions hold.
* Otherwise, it is failed.
*
* If an assertion failed during the execution, the execution is immediately terminated.
*
* Cases are collected together in test units.
* See test_unit.h
*
*/
	class test_case_info final
	{
	public:
		test_case_info() {}
		~test_case_info() {}

		inline void begin_test_case() { bStarted = true; }
		inline void end_test_case() { bEndedNormally = bStarted; } // equivalent to if(bStarted) bEndedNormally = true;

		inline bool succeeded() const { return nullptr == failed_msg && bEndedNormally; } // even if user sets the msg to nullptr even if an assertion fails, we can still tell if it succeeds from bEndedNormally.

		inline void on_failure(const char* msg) { failed_msg = msg; }

		inline auto get_failed_msg() const { return failed_msg; }

	private:
		// Since that a cases terminates on one failure, it is either nullptr or the msg of the first failed assertion.
		const char* failed_msg = nullptr;

		bool bStarted = false;
		bool bEndedNormally = false;
	};
}

/*
* For users, a test case is a void function that takes a reference of test_case_info.
* The user puts the code that will be executed in the test case in it, and makes assertions by the following 3 macros.
* 
* The parameter the function takes is used to record the info for the case (e.g. is it successful)
* 
* The user must ensure that 
* 1. the function is independent of other test cases (i.e. its result is not influenced by other test cases in any way)
* 2. the function is consistent (i.e. all calls to it give the same info)
* 
* The function should be defined by DEFINE_TEST_CASE and ENDDEF_TEST_CASE macros, and may be declared by DECLARE_TEST_CASE macro
*/

// the two macros are used to convert __LINE__ from an integer literal to a string literal
#define STRINGIFY_LINE(line) #line
#define EVALUATE_LINE(line) STRINGIFY_LINE(line)

#define ASSERT_TRUE(expr, msg) if(!(expr)) { ghl_arg.on_failure(msg " in file: " __FILE__ " at line: " EVALUATE_LINE(__LINE__)); return; }
#define ASSERT_FALSE(expr, msg) if(expr) { ghl_arg.on_failure(msg " in file: " __FILE__ " at line: " EVALUATE_LINE(__LINE__)); return; }
#define ASSERT_EQUALS(expected, actual, msg) ASSERT_TRUE(expected == actual, msg)

#define DECLARE_TEST_CASE(name) extern void name(ghl::test_case_info & ghl_arg);
#define DEFINE_TEST_CASE(name) void name(ghl::test_case_info & ghl_arg) { ghl_arg.begin_test_case();
#define ENDDEF_TEST_CASE ghl_arg.end_test_case(); }