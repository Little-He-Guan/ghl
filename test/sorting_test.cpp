#include "../algorithms/sorting.h"
#include "../unit_test/test_unit.h"

#include "../data_structures/vector.h"

#include <iostream>

DEFINE_TEST_CASE(test_sorting_bubble)

// sort an empty set or a list with one element
{
	ghl::vector<int> v_empty;
	ghl::bubble_sort(v_empty);
	ASSERT_TRUE(v_empty.empty(), "expected to do nothing")

	v_empty.push_back(2);
	ghl::bubble_sort(v_empty);
	ASSERT_EQUALS(1, v_empty.size(), "expected to do nothing")
	ASSERT_EQUALS(2, v_empty[0], "expected to do nothing")
}

// sort an already sorted list
{
	ghl::vector<int> v_sorted{ 1,2,3,4 };
	ghl::bubble_sort(v_sorted);
	ASSERT_EQUALS(4, v_sorted.size(), "expected to do nothing")
	ASSERT_EQUALS(1, v_sorted[0], "expected to do nothing")
	ASSERT_EQUALS(2, v_sorted[1], "expected to do nothing")
	ASSERT_EQUALS(3, v_sorted[2], "expected to do nothing")
	ASSERT_EQUALS(4, v_sorted[3], "expected to do nothing")
}

// sort a reversely sorted list
{
	ghl::vector<int> v_sorted{ 8,7,6,5 };
	ghl::bubble_sort(v_sorted);
	ASSERT_EQUALS(4, v_sorted.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(5, v_sorted[0], "expected to sort the list")
	ASSERT_EQUALS(6, v_sorted[1], "expected to sort the list")
	ASSERT_EQUALS(7, v_sorted[2], "expected to sort the list")
	ASSERT_EQUALS(8, v_sorted[3], "expected to sort the list")
}

// sort a partially sorted list
{
	ghl::vector<int> v_pt{ 1,7,2,3,5,4,6,8 };
	ghl::bubble_sort(v_pt);
	ASSERT_EQUALS(8, v_pt.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(1, v_pt[0], "expected to sort the list")
	ASSERT_EQUALS(2, v_pt[1], "expected to sort the list")
	ASSERT_EQUALS(3, v_pt[2], "expected to sort the list")
	ASSERT_EQUALS(4, v_pt[3], "expected to sort the list")
	ASSERT_EQUALS(5, v_pt[4], "expected to sort the list")
	ASSERT_EQUALS(6, v_pt[5], "expected to sort the list")
	ASSERT_EQUALS(7, v_pt[6], "expected to sort the list")
	ASSERT_EQUALS(8, v_pt[7], "expected to sort the list")
}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_sorting_insertion)

// sort an empty set or a list with one element
{
	ghl::vector<int> v_empty;
	ghl::insertion_sort(v_empty.begin(),v_empty.end());
	ASSERT_TRUE(v_empty.empty(), "expected to do nothing")

	v_empty.push_back(2);
	ghl::insertion_sort(v_empty.begin(),v_empty.end());
	ASSERT_EQUALS(1, v_empty.size(), "expected to do nothing")
	ASSERT_EQUALS(2, v_empty[0], "expected to do nothing")
}

// sort an already sorted list
{
	ghl::vector<int> v_sorted{ 1,2,3,4 };
	ghl::insertion_sort(v_sorted.begin(),v_sorted.end());
	ASSERT_EQUALS(4, v_sorted.size(), "expected to do nothing")
	ASSERT_EQUALS(1, v_sorted[0], "expected to do nothing")
	ASSERT_EQUALS(2, v_sorted[1], "expected to do nothing")
	ASSERT_EQUALS(3, v_sorted[2], "expected to do nothing")
	ASSERT_EQUALS(4, v_sorted[3], "expected to do nothing")
}

// sort a reversely sorted list
{
	ghl::vector<int> v_sorted{ 8,7,6,5 };
	ghl::insertion_sort(v_sorted.begin(),v_sorted.end());
	ASSERT_EQUALS(4, v_sorted.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(5, v_sorted[0], "expected to sort the list")
	ASSERT_EQUALS(6, v_sorted[1], "expected to sort the list")
	ASSERT_EQUALS(7, v_sorted[2], "expected to sort the list")
	ASSERT_EQUALS(8, v_sorted[3], "expected to sort the list")
}

// sort a partially sorted list
{
	ghl::vector<int> v_pt{ 1,7,2,3,5,4,6,8 };
	ghl::insertion_sort(v_pt.begin(),v_pt.end());
	ASSERT_EQUALS(8, v_pt.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(1, v_pt[0], "expected to sort the list")
	ASSERT_EQUALS(2, v_pt[1], "expected to sort the list")
	ASSERT_EQUALS(3, v_pt[2], "expected to sort the list")
	ASSERT_EQUALS(4, v_pt[3], "expected to sort the list")
	ASSERT_EQUALS(5, v_pt[4], "expected to sort the list")
	ASSERT_EQUALS(6, v_pt[5], "expected to sort the list")
	ASSERT_EQUALS(7, v_pt[6], "expected to sort the list")
	ASSERT_EQUALS(8, v_pt[7], "expected to sort the list")
}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_sorting_selection)

// sort an empty set or a list with one element
{
	ghl::vector<int> v_empty;
	ghl::selection_sort(v_empty);
	ASSERT_TRUE(v_empty.empty(), "expected to do nothing")

	v_empty.push_back(2);
	ghl::selection_sort(v_empty);
	ASSERT_EQUALS(1, v_empty.size(), "expected to do nothing")
	ASSERT_EQUALS(2, v_empty[0], "expected to do nothing")
}

// sort an already sorted list
{
	ghl::vector<int> v_sorted{ 1,2,3,4 };
	ghl::selection_sort(v_sorted);
	ASSERT_EQUALS(4, v_sorted.size(), "expected to do nothing")
	ASSERT_EQUALS(1, v_sorted[0], "expected to do nothing")
	ASSERT_EQUALS(2, v_sorted[1], "expected to do nothing")
	ASSERT_EQUALS(3, v_sorted[2], "expected to do nothing")
	ASSERT_EQUALS(4, v_sorted[3], "expected to do nothing")
}

// sort a reversely sorted list
{
	ghl::vector<int> v_sorted{ 8,7,6,5 };
	ghl::selection_sort(v_sorted);
	ASSERT_EQUALS(4, v_sorted.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(5, v_sorted[0], "expected to sort the list")
	ASSERT_EQUALS(6, v_sorted[1], "expected to sort the list")
	ASSERT_EQUALS(7, v_sorted[2], "expected to sort the list")
	ASSERT_EQUALS(8, v_sorted[3], "expected to sort the list")
}

// sort a partially sorted list
{
	ghl::vector<int> v_pt{ 1,7,2,3,5,4,6,8 };
	ghl::selection_sort(v_pt);
	ASSERT_EQUALS(8, v_pt.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(1, v_pt[0], "expected to sort the list")
	ASSERT_EQUALS(2, v_pt[1], "expected to sort the list")
	ASSERT_EQUALS(3, v_pt[2], "expected to sort the list")
	ASSERT_EQUALS(4, v_pt[3], "expected to sort the list")
	ASSERT_EQUALS(5, v_pt[4], "expected to sort the list")
	ASSERT_EQUALS(6, v_pt[5], "expected to sort the list")
	ASSERT_EQUALS(7, v_pt[6], "expected to sort the list")
	ASSERT_EQUALS(8, v_pt[7], "expected to sort the list")
}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_sorting_merge)

// sort an empty set or a list with one element
{
	ghl::vector<int> v_empty;
	ghl::merge_sort(v_empty);
	ASSERT_TRUE(v_empty.empty(), "expected to do nothing")

	v_empty.push_back(2);
	ghl::merge_sort(v_empty);
	ASSERT_EQUALS(1, v_empty.size(), "expected to do nothing")
	ASSERT_EQUALS(2, v_empty[0], "expected to do nothing")
}

// sort an already sorted list
{
	ghl::vector<int> v_sorted{ 1,2,3,4 };
	ghl::merge_sort(v_sorted);
	ASSERT_EQUALS(4, v_sorted.size(), "expected to do nothing")
	ASSERT_EQUALS(1, v_sorted[0], "expected to do nothing")
	ASSERT_EQUALS(2, v_sorted[1], "expected to do nothing")
	ASSERT_EQUALS(3, v_sorted[2], "expected to do nothing")
	ASSERT_EQUALS(4, v_sorted[3], "expected to do nothing")
}

// sort a reversely sorted list
{
	ghl::vector<int> v_sorted{ 8,7,6,5 };
	ghl::merge_sort(v_sorted);
	ASSERT_EQUALS(4, v_sorted.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(5, v_sorted[0], "expected to sort the list")
	ASSERT_EQUALS(6, v_sorted[1], "expected to sort the list")
	ASSERT_EQUALS(7, v_sorted[2], "expected to sort the list")
	ASSERT_EQUALS(8, v_sorted[3], "expected to sort the list")
}

// sort a partially sorted list
{
	ghl::vector<int> v_pt{ 1,7,2,3,5,4,6,8 };
	ghl::merge_sort(v_pt);
	ASSERT_EQUALS(8, v_pt.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(1, v_pt[0], "expected to sort the list")
	ASSERT_EQUALS(2, v_pt[1], "expected to sort the list")
	ASSERT_EQUALS(3, v_pt[2], "expected to sort the list")
	ASSERT_EQUALS(4, v_pt[3], "expected to sort the list")
	ASSERT_EQUALS(5, v_pt[4], "expected to sort the list")
	ASSERT_EQUALS(6, v_pt[5], "expected to sort the list")
	ASSERT_EQUALS(7, v_pt[6], "expected to sort the list")
	ASSERT_EQUALS(8, v_pt[7], "expected to sort the list")
}

ENDDEF_TEST_CASE

void test_sortings()
{
	ghl::test_unit unit
	{ 
		{
			&test_sorting_bubble,
			&test_sorting_insertion,
			&test_sorting_selection,
			&test_sorting_merge
		},
		"test for sortings" 
	};

	unit.execute();
	std::cout << unit.get_msg() << "\n";
}