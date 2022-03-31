#include "../algorithms/dynamic_programming.h"
#include "../unit_test/test_unit.h"
#include "../data_structures/vector.h"

#include <iostream>

DEFINE_TEST_CASE(test_dp_fib)

	// base case
	{
		ASSERT_EQUALS(0, ghl::fib_dp(0), "expected to have the base cases right")
		ASSERT_EQUALS(1, ghl::fib_dp(1), "expected to have the base cases right")
		ASSERT_EQUALS(1, ghl::fib_dp(1), "expected to have the base cases right")
	}

	// other cases
	{
		ASSERT_EQUALS(8, ghl::fib_dp(6), "expected to have the other cases right")
		ASSERT_EQUALS(34, ghl::fib_dp(9), "expected to have the other cases right")
		// one large one, only verfies that the invariant holds
		ASSERT_EQUALS(ghl::fib_dp(100) + ghl::fib_dp(101), ghl::fib_dp(102), "expected to have the other cases right")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_dp_assembly_line)

	// only one machine per line
	{
		ghl::vector<unsigned> first{ 1 }, second{ 2 };
		ghl::vector<unsigned> fts, stf;
		ghl::vector<bool> steps;

		auto res = ghl::assembly_line_dp(first, second, fts, stf, steps);

		ASSERT_EQUALS(1, steps.size(), "expected to only use one step")
		ASSERT_EQUALS(false, steps[0], "expected to choose the first assembly line")
		ASSERT_EQUALS(1, res, "expected to get the res right")
	}

	// multiple machines per line (including switching from f to s, from s to f, f to f, and s to s)
	{
		ghl::vector<unsigned> first{ 5, 5, 9, 4, 10, 20, 15, 15, 1, 1 }, second{ 15, 4, 3, 7, 10, 15, 20, 10, 10, 5 };
		ghl::vector<unsigned> fts{ 2,4,1,0,2,3,2,0,7 }, stf{ 1,11,2,0,1,2,4,0,8 };
		ghl::vector<bool> steps;

		auto res = ghl::assembly_line_dp(first, second, fts, stf, steps);

		ASSERT_EQUALS(10, steps.size(), "expected to use 10 steps")
		ASSERT_EQUALS(false, steps[0], "expected to get the steps right")
		ASSERT_EQUALS(true, steps[1], "expected to get the steps right")
		ASSERT_EQUALS(true, steps[2], "expected to get the steps right")
		ASSERT_EQUALS(false, steps[3], "expected to get the steps right")
		ASSERT_EQUALS(true, steps[4], "expected to get the steps right")
		ASSERT_EQUALS(true, steps[5], "expected to get the steps right")
		ASSERT_EQUALS(false, steps[6], "expected to get the steps right")
		ASSERT_EQUALS(true, steps[7], "expected to get the steps right")
		ASSERT_EQUALS(false, steps[8], "expected to get the steps right")
		ASSERT_EQUALS(false, steps[9], "expected to get the steps right")
		ASSERT_EQUALS(76, res, "expected to get the res right")
	}

ENDDEF_TEST_CASE

void test_dp()
{
	ghl::test_unit fib
	{
		{
			&test_dp_fib
		},
		"tests for dp fibonacci sequence"
	};
	ghl::test_unit ass_line
	{
		{
			&test_dp_assembly_line
		},
		"tests for dp assembly line"
	};

	fib.execute();
	std::cout << fib.get_msg() << "\n";

	ass_line.execute();
	std::cout << ass_line.get_msg() << "\n";
}