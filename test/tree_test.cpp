#include "../data_structures/tree.h"

#include "../unit_test/test_unit.h"

#include <iostream>

// Tests of the nodes, the tree interface, and the traversal interfaces are given here

// tests class fixed_size_node<T, 2> and the class tree<fixed_size_node<T, 2>>
DEFINE_TEST_CASE(test_binary_node_and_tree)

	int* pl = new int(3), * pr = new int(4), *po = new int(1);

	auto *plft = new ghl::binary_node<int>(nullptr, pl);
	auto *pnode = new ghl::binary_node<int>(nullptr, po, plft, nullptr);

	ghl::binary_tree<int> tree(pnode);

	{
		ASSERT_FALSE(tree.empty(), "Expected to get a non-empty tree")

		ASSERT_EQUALS(3, tree.left()->get_obj(), "expected to have left obj correct")
		ASSERT_TRUE(tree.right_empty(), "expected to get an empty right")
	}

	{
		tree.set_right(pr);

		ASSERT_EQUALS(4, tree.right()->get_obj(), "expected to have right obj correct")

		tree.reset_left();
		tree.reset_right();
		tree.reset_object();

		ASSERT_TRUE(tree.empty(), "expected to have an empty tree now")
	}

ENDDEF_TEST_CASE

void test_tree_and_node()
{
	ghl::test_unit unit
	{
		{
			&test_binary_node_and_tree
		},
		"tests for binary node and tree"
	};

	unit.execute();

	std::cout << unit.get_msg() << "\n";
}