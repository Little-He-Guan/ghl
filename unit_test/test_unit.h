#pragma once

#include <utility>
#include <type_traits>
#include <list>
#include <string>
#include <initializer_list>

#include "test_case.h"

namespace ghl
{
	/*
	* A test unit consists of test cases that test the same unit of the software, and is executed as a whole.
	* 
	* The order in which the cases contained are executed is undefined.
	*/
	class test_unit final
	{
	public:
		test_unit() {}
		explicit test_unit(const std::string& info) : msg(info) {}
		// support for list-initialization
		explicit test_unit(const std::initializer_list<void (*)(test_case_info&)>& list, const std::string& info = "");

		~test_unit() {}

		/*
		* Adds a test case for execution
		* 
		* @param pf pointer to the function that defines the test case
		*/
		inline void add_test_case(void (*pf)(test_case_info&)) { test_cases.push_back(std::pair<test_case_info, void (*)(test_case_info&)>{ {}, pf }); }

		/*
		* Executes the test cases and produces the result message, which is contained in msg
		*/
		void execute();
		
		const std::string& get_msg() const { return msg; }

	private:
		std::list<std::pair<test_case_info, void (*)(test_case_info&)>> test_cases;
		std::string msg;

		// 33 for yellow, 32 for green
		static constexpr const char* msg_format_str = "Test unit: \"\033[0;33;40m%s\033[0m\" executed with %zu test cases, \033[0;32;40m%zu succeeded\033[0m.";
		static constexpr auto msg_format_str_len = std::char_traits<char>::length(msg_format_str);

		// 31 for red
		static constexpr const char* failed_msg = "\n\t \033[0;31;40mA test case failed with msg\033[0m ";
		static constexpr auto failed_msg_len = std::char_traits<char>::length(failed_msg);
	};
}