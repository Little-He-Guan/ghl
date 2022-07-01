#pragma once

#include "../unit_test/test_unit.h"

// we assume that size(), empty(), and contains() works correctly

DEFINE_TEST_CASE_TEMPLATE(test_set_add)

	INTERFACE set;

	// add an element into an empty set
	{
		ASSERT_TRUE(set.add(new int(3)), "expected to return true")

		ASSERT_EQUALS(1, set.size(), "expected to have the size increased")
		ASSERT_TRUE(set.contains(3), "expected to contain the element")
	}

	// add an element into a non-empty set
	{
		ASSERT_TRUE(set.add(new int(5)), "expected to return true")

		ASSERT_EQUALS(2, set.size(), "expected to have the size increased")
		ASSERT_TRUE(set.contains(5), "expected to contain the element")
	}

	// add an already existing element
	{
		ASSERT_FALSE(set.add(new int(3)), "expected to return false")

		ASSERT_EQUALS(2, set.size(), "expected to not change the set")
	}

ENDDEF_TEST_CASE_TEMPLATE

// if add works normally, we can then test remove
DEFINE_TEST_CASE_TEMPLATE(test_set_remove)

	INTERFACE set;

	// remove an element from an empty set
	{
		ASSERT_FALSE(set.remove(1), "expected to return false")
		ASSERT_EQUALS(0, set.size(), "expected to not have the set modified")
	}

	// remove an existing element from a set
	{
		set.add(new int(4));
		set.add(new int(6));

		ASSERT_TRUE(set.remove(6), "expected to return true")
		ASSERT_EQUALS(1, set.size(), "expected to not have the size decreased")
	}

	// remove a non-existing element from a non-empty set
	{
		ASSERT_FALSE(set.remove(5), "expected to return false")
		ASSERT_EQUALS(1, set.size(), "expected to not have the set modified")
	}

ENDDEF_TEST_CASE_TEMPLATE

// if add works normally, we can then test any_element
DEFINE_TEST_CASE_TEMPLATE(test_set_any_element)

	INTERFACE set;

	// gets any element from an empty set
	{
		ASSERT_EQUALS(nullptr, set.any_element(), "expected to return nullptr")

		ASSERT_EQUALS(0, set.size(), "expected to not have the size decreased")
	}

	// when there's only one element
	{
		set.add(new int(7));

		ASSERT_EQUALS(7, *(set.any_element()), "expected to have the element")
		ASSERT_EQUALS(0, set.size(), "expected to have the size decreased")
	}

	// when there are many elements
	{
		set.add(new int(11));
		set.add(new int(4));
		set.add(new int(9));

		auto p_ele = set.any_element();
		ASSERT_TRUE(*p_ele == 11 || *p_ele == 4 || *p_ele == 9, "expected to have any of the elements")
		ASSERT_EQUALS(2, set.size(), "expected to have the size decreased")
	}

ENDDEF_TEST_CASE_TEMPLATE