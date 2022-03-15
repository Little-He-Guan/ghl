#include "../unit_test/test_unit.h"

#include <iostream>

DEFINE_TEST_CASE(test_test_1)

	ASSERT_TRUE(true, "")
	ASSERT_FALSE(false, "")

	int a = 2;

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_test_2)

	ASSERT_TRUE(true, "")
	ASSERT_FALSE(false, "")

	int a = 2, b = 3;

	ASSERT_EQUALS(a, b, "shit!")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_test_3)

	int a = 2, b = 3;

	ASSERT_EQUALS(a, b, "fuck!")

ENDDEF_TEST_CASE

void test_vector();

int main()
{
	//ghl::test_unit unit{ {&test_test_1,&test_test_2}, "damn" };

	//unit.add_test_case(&test_test_3);
	//unit.execute();

	test_vector();

	static constexpr int arr[3] ={1,2,3};

	constexpr const int* p1 = arr, *p2 = p1 + 2;
	constexpr auto n = p2 - p1;

	return 0;
}