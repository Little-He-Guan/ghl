#include "../data_structures/binary_search_tree.h"
#include "../unit_test/test_unit.h"

#include "../data_structures/vector.h"

#include <iostream>

DEFINE_TEST_CASE(test_binary_search_tree_insert)

	ghl::binary_search_tree<int> bst;

	// the tree requires the element to be allocated dynamically
	ghl::vector<int*> data_to_be_inserted(10);
	for (int i = 1; i <= 10; ++i)
	{
		data_to_be_inserted.push_back(new int(i));
	}
	
	// insert into an empty tree
	{
		bst.insert(data_to_be_inserted[6 - 1]);

		ASSERT_FALSE(bst.empty(), "expected to have the element inserted")
		ASSERT_EQUALS(6, bst.get_root()->get_obj(), "expected to have the inserted element correct")
		ASSERT_TRUE(bst.left_empty() && bst.right_empty(), "expected to not modify the branches")
	}

	// insert into a non-empty tree
	{
		// insert a one smaller than root
		bst.insert(data_to_be_inserted[1 - 1]);
		ASSERT_FALSE(bst.left_empty(), "it should be inserted onto left")
		ASSERT_EQUALS(1, bst.left()->get_obj(), "expected to get the value correct")

		// insert a one bigger than min and smaller than root
		bst.insert(data_to_be_inserted[2 - 1]);
		ASSERT_FALSE(bst.left()->branch_empty(1), "it should be inserted onto the right of 1")
		ASSERT_EQUALS(2, bst.left()->right()->get_obj(), "expected to get the value correct")

		// insert a one bigger than root
		bst.insert(data_to_be_inserted[10 - 1]);
		ASSERT_FALSE(bst.right_empty(), "it should be inserted onto right")
		ASSERT_EQUALS(10, bst.right()->get_obj(), "expected to get the value correct")

		// insert a one smaller than max and bigger than root
		bst.insert(data_to_be_inserted[9 - 1]);
		ASSERT_FALSE(bst.right()->branch_empty(0), "it should be inserted onto the left of 10")
		ASSERT_EQUALS(9, bst.right()->left()->get_obj(), "expected to get the value correct")

		// insert in descending order
		bst.insert(data_to_be_inserted[5 - 1]);
		bst.insert(data_to_be_inserted[4 - 1]);
		bst.insert(data_to_be_inserted[3 - 1]);
		// set node to 2
		auto* node = bst.left()->right();
		ASSERT_FALSE(node->branch_empty(1), "5 should be inserted onto the right of node")
		ASSERT_EQUALS(5, node->right()->get_obj(), "expected to get the value correct")
		node = node->right();
		ASSERT_FALSE(node->branch_empty(0), "4 should be inserted onto the left of node")
		ASSERT_EQUALS(4, node->left()->get_obj(), "expected to get the value correct")
		node = node->left();
		ASSERT_FALSE(node->branch_empty(0), "3 should be inserted onto the left of node")
		ASSERT_EQUALS(3, node->left()->get_obj(), "expected to get the value correct")

		// insert in ascending order
		bst.insert(data_to_be_inserted[7 - 1]);
		bst.insert(data_to_be_inserted[8 - 1]);
		// set node to 9
		node = bst.right()->left();
		ASSERT_FALSE(node->branch_empty(0), "7 should be inserted onto the left of node")
		ASSERT_EQUALS(7, node->left()->get_obj(), "expected to get the value correct")
		node = node->left();
		ASSERT_FALSE(node->branch_empty(1), "8 should be inserted onto the right of node")
		ASSERT_EQUALS(8, node->right()->get_obj(), "expected to get the value correct")
	}

	// insert elements that is already in the tree
	{
		int* p5 = new int(5);

		bst.insert(p5);
		// we can make no assertions about where it's going to be inserted,
		// we can only check if the binary search tree property still holds
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_binary_search_tree_find)

	ghl::binary_search_tree<int> bst;

	// the tree requires the element to be allocated dynamically
	ghl::vector<int*> data_to_be_inserted(10);
	for (int i = 1; i <= 10; ++i)
	{
		data_to_be_inserted.push_back(new int(i));
	}

	// find something in an empty tree
	{
		ASSERT_FALSE(bst.find(1).valid(), "expected to find nothing")
	}

	// let the bst be as high and complex as possible
	{
		bst.insert(data_to_be_inserted[6 - 1]);
		bst.insert(data_to_be_inserted[3 - 1]);
		bst.insert(data_to_be_inserted[4 - 1]);
		bst.insert(data_to_be_inserted[5 - 1]);
		bst.insert(data_to_be_inserted[2 - 1]);
		bst.insert(data_to_be_inserted[1 - 1]);
		bst.insert(data_to_be_inserted[8 - 1]);
		bst.insert(data_to_be_inserted[7 - 1]);
		bst.insert(data_to_be_inserted[9 - 1]);
		bst.insert(data_to_be_inserted[10 - 1]);
	}

	// find something in the tree
	{
		auto iter = bst.find(1);
		ASSERT_TRUE(iter.valid(), "expected to find something")
		ASSERT_EQUALS(1, iter->get_obj(), "expected to find the correct one")
	}

	// find something not in the tree
	{
		ASSERT_FALSE(bst.find(99).valid(), "expected to find nothing")
	}

ENDDEF_TEST_CASE

/*
* Maximum and minimun are simple so we don't test them here.
*/

DEFINE_TEST_CASE(test_binary_search_tree_remove)

	ghl::binary_search_tree<int> bst;

	// the tree requires the element to be allocated dynamically
	ghl::vector<int*> data_to_be_inserted(10);
	for (int i = 1; i <= 10; ++i)
	{
		data_to_be_inserted.push_back(new int(i));
	}

	// try to remove something from an empty tree
	{
		ASSERT_FALSE(bst.remove(1), "expected to return false");
	}

	// let the bst be as high and complex as possible
	{
		bst.insert(data_to_be_inserted[6 - 1]);
		bst.insert(data_to_be_inserted[3 - 1]);
		bst.insert(data_to_be_inserted[4 - 1]);
		bst.insert(data_to_be_inserted[5 - 1]);
		bst.insert(data_to_be_inserted[2 - 1]);
		bst.insert(data_to_be_inserted[1 - 1]);
		bst.insert(data_to_be_inserted[8 - 1]);
		bst.insert(data_to_be_inserted[7 - 1]);
		bst.insert(data_to_be_inserted[9 - 1]);
		bst.insert(data_to_be_inserted[10 - 1]);
	}

	// try to remove something not in the tree
	{
		ASSERT_FALSE(bst.remove(99), "expected to return false");
	}
	
	// try to remove something who has no children (then it will become nullptr simply)
	{
		ASSERT_TRUE(bst.remove(10), "expected to return true")
		ASSERT_FALSE(bst.find(10).valid(), "expected to remove the element")

		auto iter = bst.maximum();
		ASSERT_EQUALS(9, iter->get_obj(), "expected to have 9 to be the new maximum")
		ASSERT_TRUE(iter->branch_empty(1), "expected to have its right nullptr (10 successfully removed)")
	}

	// try to remove something who has one child (then that child will take its place)
	{
		ASSERT_TRUE(bst.remove(2), "expected to return true")
		ASSERT_FALSE(bst.find(2).valid(), "expected to remove the element")

		auto * p3 = bst.left();
		ASSERT_FALSE(p3->branch_empty(0), "its place should be taken")
		ASSERT_EQUALS(1, p3->left()->get_obj(), "its child should take its place")
	}
	
	// try to remove something who has two children (then its successor will take its place and the tree will be rearranged if necessary)

	// no rearrangement (when its successor is one of its children)
	{
		ASSERT_TRUE(bst.remove(8), "expected to return true")
		ASSERT_FALSE(bst.find(8).valid(), "expected to remove the element")

		auto * p9 = bst.right();
		ASSERT_EQUALS(9, p9->get_obj(), "its child should take its place")
		ASSERT_FALSE(p9->branch_empty(0), "its another child should not be modified")
		ASSERT_EQUALS(7, p9->left()->get_obj(), "its another child should not be modified")
	}

	// rearrangement occurs (when its successor is not one of its direct children)
	{
		// insert 8, which will be the new right of 7, so that we can check if the rearrangement happens
		bst.insert(new int(8));

		ASSERT_TRUE(bst.remove(6), "expected to return true")
		ASSERT_FALSE(bst.find(6).valid(), "expected to remove the element")

		ASSERT_EQUALS(7, bst.get_root()->get_obj(), "expected to have its successor replace its place")
		ASSERT_FALSE(bst.right()->branch_empty(0), "7's original right child should be rearranged")
		ASSERT_EQUALS(8, bst.right()->left()->get_obj(), "7's original right child should be modified")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_binary_search_tree_successor_predecessor)

	ghl::binary_search_tree<int> bst;

	// the tree requires the element to be allocated dynamically
	ghl::vector<int*> data_to_be_inserted(10);
	for (int i = 1; i <= 10; ++i)
	{
		data_to_be_inserted.push_back(new int(i));
	}

	// when there is no successor/predecessor
	{
		bst.insert(data_to_be_inserted[6 - 1]);
		
		auto i6 = bst.get_root_iter();
		ASSERT_FALSE(i6.successor().valid(), "expected to get an invalid iter")
		ASSERT_FALSE(i6.predecessor().valid(), "expected to get an invalid iter")
	}

	// insert some other elements to make the tree complex
	{
		bst.insert(data_to_be_inserted[3 - 1]);
		bst.insert(data_to_be_inserted[4 - 1]);
		bst.insert(data_to_be_inserted[5 - 1]);
		bst.insert(data_to_be_inserted[2 - 1]);
		bst.insert(data_to_be_inserted[1 - 1]);
		bst.insert(data_to_be_inserted[8 - 1]);
		bst.insert(data_to_be_inserted[7 - 1]);
		bst.insert(data_to_be_inserted[9 - 1]);
		bst.insert(data_to_be_inserted[10 - 1]);
	}

	// when the successor/predecessor is its direct child
	{
		auto i9 = bst.find(9);
		auto s9 = i9.successor();
		ASSERT_TRUE(s9.valid() ? s9->get_obj() == 10 : false, "expected to get the correct successor")

		auto i2 = bst.find(2);
		auto p2 = i2.predecessor();
		ASSERT_TRUE(p2.valid() ? p2->get_obj() == 1 : false, "expected to get the correct predecessor")
	}

	// when the successor/predecessor is its direct parent
	{
		auto i5 = bst.find(5);
		auto p5 = i5.predecessor();
		ASSERT_TRUE(p5.valid() ? p5->get_obj() == 4 : false, "expected to get the correct predecessor")

		auto i7 = bst.find(7);
		auto s7 = i7.successor();
		ASSERT_TRUE(s7.valid() ? s7->get_obj() == 8 : false, "expected to get the correct successor")
	}

	// when the successor/predecessor is not adjacent to it
	{
		auto i6 = bst.find(6);
		auto p6 = i6.predecessor();
		ASSERT_TRUE(p6.valid() ? p6->get_obj() == 5 : false, "expected to get the correct predecessor")

		auto i5 = bst.find(5);
		auto s5 = i5.successor();
		ASSERT_TRUE(s5.valid() ? s5->get_obj() == 6 : false, "expected to get the correct successor")
	}

ENDDEF_TEST_CASE

void test_bst()
{
	ghl::test_unit unit
	{
		{
			&test_binary_search_tree_insert,
			&test_binary_search_tree_find,
			&test_binary_search_tree_remove,
			&test_binary_search_tree_successor_predecessor
		},
		"tests for bst"
	};

	unit.execute();

	std::cout << unit.get_msg() << "\n";
}