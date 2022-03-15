#include "pch.h"
#include "test_unit.h"

ghl::test_unit::test_unit(const std::initializer_list<void(*)(test_case_info&)>& list, const std::string& info)
	: test_unit(info)
{
	for (auto p : list)
	{
		test_cases.push_back(std::pair<test_case_info, void (*)(test_case_info&)>{ {}, p });
	}
}

void ghl::test_unit::execute()
{
	size_t num_successes = 0;

	// allocate the memory of the msg in one go
	// 8 = len(%s%zu%zu)
	auto msg_size_expected = msg_format_str_len - 8 + msg.size() + std::to_string(test_cases.size()).size();
	auto msg_size_without_errors = msg_size_expected;

	// execute the test cases
	for (auto& tc : test_cases)
	{
		tc.second(tc.first);

		if (tc.first.succeeded())
		{
			++num_successes;
		}
		else
		{
			msg_size_expected += failed_msg_len + std::char_traits<char>::length(tc.first.get_failed_msg());
		}
	}
	msg_size_expected += std::to_string(num_successes).size();
	msg_size_without_errors += std::to_string(num_successes).size();
	
	// the initial info is stored in msg initially
	auto info_cpy = msg;

	msg.clear();
	msg.reserve(msg_size_expected);

	// change the size in advance
	msg.resize(msg_size_without_errors);
	// write the message without the errors
	// the size needs to + 1 because sprintf_s writes one extra \0
	// the \0 will be overwritten by the += operations below
	sprintf_s(&msg[0], msg_size_without_errors + 1, msg_format_str, info_cpy.c_str(), test_cases.size(), num_successes);

	// write the error messages
	for (const auto& tc : test_cases)
	{
		if (!tc.first.succeeded())
		{
			msg += failed_msg;
			msg += tc.first.get_failed_msg();
		}
	}
}
