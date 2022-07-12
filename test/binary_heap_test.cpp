#include "../unit_test/test_unit.h"

#include "../data_structures/binary_heap.h"

#include <iostream>

// we assume that operator[](), size(), and consequently top() and empty() work normally
//
// since lw and hw heaps differ only in storing objects or pointers, we only write tests for lw obj heaps.

namespace ghl
{
	class max_heap_lw_tester
	{
	public:
		template <typename T>
		static auto& get_data(ghl::max_heap_lw<T>& heap) { return heap.data; }

		template <typename T>
		static void heapify(ghl::max_heap_lw<T>& heap, size_t i) { heap.heapify(i); }
	};
}

/*
* Checks if a binary heap satisfy the heap property
* 
* @param max_min true for max heap property, false for min
* 
* @returns true iff the property holds
*/
template <typename T>
bool satisfy_heap_property_lw(ghl::binary_heap<T>& heap, bool max_min)
{
	using heap_t = ghl::binary_heap<T>;

	size_t size = heap.size();
	if (size > 1)
	{
		// check for all nodes except the root
		for (size_t i = 2; i <= size; ++i)
		{
			if (max_min) // max
			{
				if (heap[heap_t::parent(i)] < heap[i]) return false;
			}
			else // min
			{
				if (heap[heap_t::parent(i)] < heap[i]) return false;
			}
		}

		return true;
	}
	else
	{
		return true;
	}
}

// works the same as the previous one, except for that it is for hw heaps
template <typename T>
bool satisfy_heap_property_hw(ghl::binary_heap<T>& heap, bool max_min)
{
	using heap_t = ghl::binary_heap<T>;

	size_t size = heap.size();
	if (size > 1)
	{
		// check for all nodes except the root
		for (size_t i = 2; i <= size; ++i)
		{
			if (max_min) // max
			{
				if (*heap[heap_t::parent(i)] < *heap[i]) return false;
			}
			else // min
			{
				if (*heap[heap_t::parent(i)] < *heap[i]) return false;
			}
		}

		return true;
	}
	else
	{
		return true;
	}
}

DEFINE_TEST_CASE(test_max_heap_lw_insert)

	// we can only test whether the heap will satisfy the property after an insertion

	ghl::max_heap_lw<int> heap;

	// insert into an empty heap
	{
		heap.insert(10);

		ASSERT_FALSE(heap.empty(), "expected to make it empty no longer")
		ASSERT_EQUALS(1, heap.size(), "expected to have its size increase by 1")
		ASSERT_TRUE(satisfy_heap_property_lw(heap,true), "expected to maintain the property")
		ASSERT_EQUALS(10, heap.top(), "expected to have the new max be the inserted one")
	}

	// insert one that is bigger than root
	{
		heap.insert(20);

		ASSERT_EQUALS(2, heap.size(), "expected to have its size increase by 1")
		ASSERT_TRUE(satisfy_heap_property_lw(heap,true), "expected to maintain the property")
		ASSERT_EQUALS(20, heap.top(), "expected to have the new max be the inserted one")
	}

	// insert one that will not break the property
	{
		heap.insert(8);

		ASSERT_EQUALS(3, heap.size(), "expected to have its size increase by 1")
		ASSERT_TRUE(satisfy_heap_property_lw(heap, true), "expected to maintain the property")
		ASSERT_EQUALS(20, heap.top(), "expected to have the top unmodified")
	}

	// insert one that will break the property at some subtree, not the entire tree (i.e. not bigger than root)
	{
		heap.insert(15);

		ASSERT_EQUALS(4, heap.size(), "expected to have its size increase by 1")
		ASSERT_TRUE(satisfy_heap_property_lw(heap, true), "expected to maintain the property")
		ASSERT_EQUALS(20, heap.top(), "expected to have the top unmodified")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_max_heap_lw_heapify)

	ghl::max_heap_lw<int> heap;
	auto& data = ghl::max_heap_lw_tester::get_data(heap);

	// when the property already holds
	{
		data.push_back(10);
		data.push_back(5);
		data.push_back(7);

		ghl::max_heap_lw_tester::heapify(heap, 1);

		ASSERT_TRUE(data[0] == 10 && data[1] == 5 && data[2] == 7, "expected not to do anything")
	}

	// when root is smaller than one of its direct children 
	{
		std::swap(data[0], data[2]);

		ghl::max_heap_lw_tester::heapify(heap, 1);

		ASSERT_TRUE(satisfy_heap_property_lw(heap, true), "expected to maintain the property")
	}

	// when root is smaller than all of its direct children (and we expect the biggest of the two to become the new root)
	{
		data.clear();
		data.push_back(1);
		data.push_back(2);
		data.push_back(3);

		ghl::max_heap_lw_tester::heapify(heap, 1);

		ASSERT_TRUE(satisfy_heap_property_lw(heap, true), "expected to maintain the property")
	}

	// when root is smaller than some of its direct children, and is still smaller than some of its indirect children 
	// (we expect the process to continue then)
	{
		data.clear();
		data.push_back(1);
		data.push_back(7);
		data.push_back(8);
		data.push_back(2);
		data.push_back(3);
		data.push_back(4);
		data.push_back(5);

		ghl::max_heap_lw_tester::heapify(heap, 1);

		ASSERT_TRUE(satisfy_heap_property_lw(heap, true), "expected to maintain the property")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_max_heap_lw_extract_top)

	ghl::max_heap_lw<int> heap;

	// when the extraction does not lead to the property being violated
	// (i.e. when there is only one element)
	{
		heap.insert(10);

		ASSERT_EQUALS(10, heap.extract_top(), "expected to have the element right")
		ASSERT_TRUE(heap.empty(), "expected to make the heap empty now")
	}

	// when the extraction leads to the property being violated
	{
		heap.insert(9);
		heap.insert(5);
		heap.insert(7);
		heap.insert(8);
		heap.insert(6);
		heap.insert(3);

		ASSERT_EQUALS(9, heap.extract_top(), "expected to have the element right")
		ASSERT_EQUALS(5, heap.size(), "expected to decrease the size by 1")
		ASSERT_TRUE(satisfy_heap_property_lw(heap, true), "expected to maintain the property")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_max_heap_lw_update_element)

	ghl::max_heap_lw<int> heap;

	// when the update does not lead to a violation to the property
	{
		heap.insert(5);

		heap.update_element(1, 20);

		ASSERT_EQUALS(1, heap.size(), "expected to have the size unchanged")
		ASSERT_EQUALS(20, heap.top(), "expected to update the element correctly")
	}

	// when the update leads to a violation to the property
	{
		heap.insert(15);
		heap.insert(16);
		heap.insert(6);
		heap.insert(5);
		heap.insert(9);
		heap.insert(7);

		heap.update_element(5, 30);

		ASSERT_EQUALS(7, heap.size(), "expected to have the size unchanged")
		ASSERT_EQUALS(30, heap.top(), "expected to update the element correctly")
		ASSERT_TRUE(satisfy_heap_property_lw(heap, true), "expected to maintain the property")
	}

ENDDEF_TEST_CASE

void test_binary_heap()
{
	ghl::test_unit max_unit
	{
		{
			&test_max_heap_lw_insert,
			&test_max_heap_lw_heapify,
			&test_max_heap_lw_extract_top,
			&test_max_heap_lw_update_element
		},
		"tests for max heap"
	};

	max_unit.execute();

	std::cout << max_unit.get_msg() << "\n";
}