#include "../data_structures/avl_tree.h"
#include "../unit_test/test_unit.h"

#include "../data_structures/vector.h"

#include <iostream>

namespace ghl
{
	class avl_tree_tester
	{
	public:
		template <typename T, typename P>
		static auto call_check_balance_on_path(avl_tree<T>& tree, P param)
		{
			return tree.check_balance_on_path(param);
		}
	};
}

using C = ghl::binary_tree_with_height<int>;

DEFINE_TEST_CASE(test_avl_tree_check_imbalance_on_path)

	// when there is only root and no branches
	{
		auto *root = new C(new int(3));
		ghl::avl_tree<int> tree(root);

		auto info = ghl::avl_tree_tester::call_check_balance_on_path(tree, tree.get_root_iter());
		ASSERT_TRUE(info.balanced, "expected to say it's balanced")
	}

	// when all subtrees on the path are balanced
	{
		// the tree requires the element to be allocated dynamically
		ghl::vector<int*> data_to_be_inserted(7);
		for (int i = 1; i <= 7; ++i)
		{
			data_to_be_inserted.push_back(new int(i));
		}

		auto* root = new C(data_to_be_inserted[4-1]);
		root->emplace_left(data_to_be_inserted[2 - 1]);
		root->left<C>()->emplace_left(data_to_be_inserted[1 - 1]);
		root->left<C>()->emplace_right(data_to_be_inserted[3 - 1]);
		root->emplace_right(data_to_be_inserted[6 - 1]);
		root->right<C>()->emplace_left(data_to_be_inserted[5 - 1]);
		root->right<C>()->emplace_right(data_to_be_inserted[7 - 1]);

		ghl::avl_tree<int> tree(root);

		auto info = ghl::avl_tree_tester::call_check_balance_on_path(tree, tree.find(1));
		ASSERT_TRUE(info.balanced, "expected to say it's balanced")
	}

	// when imbalance happens
	{
		// LL
		{
			ghl::vector<int*> data_to_be_inserted(6);
			for (int i = 1; i <= 6; ++i)
			{
				data_to_be_inserted.push_back(new int(i));
			}

			auto* root = new C(data_to_be_inserted[5 - 1]);
			root->emplace_left(data_to_be_inserted[3 - 1]);
			root->left<C>()->emplace_left(data_to_be_inserted[2 - 1]);
			root->left<C>()->emplace_right(data_to_be_inserted[4 - 1]);
			root->left<C>()->left<C>()->emplace_left(data_to_be_inserted[1 - 1]);
			root->emplace_right(data_to_be_inserted[6 - 1]);

			ghl::avl_tree<int> tree(root);

			// when pos has right
			{
				auto info = ghl::avl_tree_tester::call_check_balance_on_path(tree, tree.find(1));
				ASSERT_FALSE(info.balanced, "expected to say it's imbalanced")
				ASSERT_EQUALS(ghl::avl_tree_imbalance_type::LL, info.type, "expected to get the type correct")
				ASSERT_EQUALS(tree.get_root(), info.pos, "expected to have the location correct")
			}

			// when pos doesn't have right
			{
				// remove 4
				root->left<C>()->reset_right();

				auto info = ghl::avl_tree_tester::call_check_balance_on_path(tree, tree.find(1));
				ASSERT_FALSE(info.balanced, "expected to say it's imbalanced")
				ASSERT_EQUALS(ghl::avl_tree_imbalance_type::LL, info.type, "expected to get the type correct")
				ASSERT_EQUALS(tree.find(3).node, info.pos, "expected to have the location correct")
			}
		}

		// LR
		{
			ghl::vector<int*> data_to_be_inserted(6);
			for (int i = 1; i <= 6; ++i)
			{
				data_to_be_inserted.push_back(new int(i));
			}

			auto* root = new C(data_to_be_inserted[5 - 1]);
			root->emplace_left(data_to_be_inserted[2 - 1]);
			root->left<C>()->emplace_left(data_to_be_inserted[1 - 1]);
			root->left<C>()->emplace_right(data_to_be_inserted[3 - 1]);
			root->left<C>()->right<C>()->emplace_right(data_to_be_inserted[4 - 1]);
			root->emplace_right(data_to_be_inserted[6 - 1]);

			ghl::avl_tree<int> tree(root);

			// when pos has right
			{
				auto info = ghl::avl_tree_tester::call_check_balance_on_path(tree, tree.find(4));
				ASSERT_FALSE(info.balanced, "expected to say it's imbalanced")
				ASSERT_EQUALS(ghl::avl_tree_imbalance_type::LR, info.type, "expected to get the type correct")
				ASSERT_EQUALS(tree.get_root(), info.pos, "expected to have the location correct")
			}

			// when pos doesn't have right
			{
				// remove 1
				root->left<C>()->reset_left(); 
				// remove 4
				root->left<C>()->right<C>()->reset_right();
				// remove 6
				root->reset_right();

				auto info = ghl::avl_tree_tester::call_check_balance_on_path(tree, tree.find(3));
				ASSERT_FALSE(info.balanced, "expected to say it's imbalanced")
				ASSERT_EQUALS(ghl::avl_tree_imbalance_type::LR, info.type, "expected to get the type correct")
				ASSERT_EQUALS(tree.get_root(), info.pos, "expected to have the location correct")
			}
		}

		// RL
		{
			ghl::vector<int*> data_to_be_inserted(6);
			for (int i = 1; i <= 6; ++i)
			{
				data_to_be_inserted.push_back(new int(i));
			}

			auto* root = new C(data_to_be_inserted[2 - 1]);
			root->emplace_left(data_to_be_inserted[1 - 1]);
			root->emplace_right(data_to_be_inserted[5- 1]);
			root->right<C>()->emplace_right(data_to_be_inserted[6 - 1]);
			root->right<C>()->emplace_left(data_to_be_inserted[3 - 1]);
			root->right<C>()->left<C>()->emplace_right(data_to_be_inserted[4 - 1]);

			ghl::avl_tree<int> tree(root);

			auto info = ghl::avl_tree_tester::call_check_balance_on_path(tree, tree.find(4));
			ASSERT_FALSE(info.balanced, "expected to say it's imbalanced")
			ASSERT_EQUALS(ghl::avl_tree_imbalance_type::RL, info.type, "expected to get the type correct")
			ASSERT_EQUALS(tree.get_root(), info.pos, "expected to have the location correct")
		}

		// RR
		{
			ghl::vector<int*> data_to_be_inserted(5);
			for (int i = 1; i <= 5; ++i)
			{
				data_to_be_inserted.push_back(new int(i));
			}

			auto* root = new C(data_to_be_inserted[2 - 1]);
			root->emplace_left(data_to_be_inserted[1 - 1]);
			root->emplace_right(data_to_be_inserted[3 - 1]);
			root->right<C>()->emplace_right(data_to_be_inserted[4 - 1]);
			root->right<C>()->right<C>()->emplace_right(data_to_be_inserted[5 - 1]);

			ghl::avl_tree<int> tree(root);

			auto info = ghl::avl_tree_tester::call_check_balance_on_path(tree, tree.find(5));
			ASSERT_FALSE(info.balanced, "expected to say it's imbalanced")
			ASSERT_EQUALS(ghl::avl_tree_imbalance_type::RR, info.type, "expected to get the type correct")
			ASSERT_EQUALS(tree.find(3).node, info.pos, "expected to have the location correct")
		}
	}

ENDDEF_TEST_CASE

/*
* as insert is but a combination of bst::insert, checking balance, and rotation,
* testing insert here comprehensively also tests rotation
*/
DEFINE_TEST_CASE(test_avl_tree_insert)

	ghl::vector<int*> data_to_be_inserted(15);
	for (int i = 1; i <= 15; ++i)
	{
		data_to_be_inserted.push_back(new int(i));
	}
	
	ghl::avl_tree<int> tree;

	// insert into an empty tree
	{
		tree.insert(data_to_be_inserted[12 - 1]);
		ASSERT_EQUALS(12, tree.get_root()->get_obj(), "expected to have the obj inserted at root")
	}

	// insertion that does not cause an imbalance
	{
		tree.insert(data_to_be_inserted[8 - 1]);
		ASSERT_EQUALS(8, tree.get_root()->left()->get_obj(), "expected to have the obj inserted corrected")
	}

	// LL
	{
		tree.insert(data_to_be_inserted[4 - 1]);

		ASSERT_EQUALS(8, tree.get_root()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(4, tree.get_root()->left()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(12, tree.get_root()->right()->get_obj(), "expected to have the elements adjusted correctly")
	}

	// LR
	{
		// release 12
		tree.get_root()->release_right();
		tree.insert(data_to_be_inserted[6 - 1]);

		ASSERT_EQUALS(6, tree.get_root()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(4, tree.get_root()->left()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(8, tree.get_root()->right()->get_obj(), "expected to have the elements adjusted correctly")
	}

	// RL
	{
		// release 8
		tree.get_root()->release_right();

		tree.insert(data_to_be_inserted[10 - 1]);
		tree.insert(data_to_be_inserted[12 - 1]);
		tree.insert(data_to_be_inserted[8 - 1]);
		tree.insert(data_to_be_inserted[7 - 1]);

		ASSERT_EQUALS(8, tree.get_root()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(6, tree.get_root()->left()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(10, tree.get_root()->right()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(4, tree.get_root()->left()->left()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(7, tree.get_root()->left()->right()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(12, tree.get_root()->right()->right()->get_obj(), "expected to have the elements adjusted correctly")
	}

	// RR
	{
		tree.insert(data_to_be_inserted[13 - 1]);

		auto i12 = tree.find(12);
		ASSERT_EQUALS(8, i12->get_parent()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(10, i12->left()->get_obj(), "expected to have the elements adjusted correctly")
		ASSERT_EQUALS(13, i12->right()->get_obj(), "expected to have the elements adjusted correctly")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_avl_tree_remove)

	ghl::avl_tree<int> tree;

	// the tree requires the element to be allocated dynamically
	ghl::vector<int*> data_to_be_inserted(7);
	for (int i = 1; i <= 7; ++i)
	{
		data_to_be_inserted.push_back(new int(i));
	}

	// remove root
	{
		tree.insert(new int(4));
		ASSERT_TRUE(tree.remove(4), "expected to return true");
		ASSERT_EQUALS(nullptr, tree.get_root(), "expected to have an empty tree")
	}

	// remove something that will not cause an imbalance
	{
		tree.insert(data_to_be_inserted[4 - 1]);
		tree.insert(data_to_be_inserted[6 - 1]);
		tree.insert(data_to_be_inserted[2 - 1]);
		tree.insert(data_to_be_inserted[1 - 1]);
		tree.insert(data_to_be_inserted[3 - 1]);

		ASSERT_TRUE(tree.remove(3), "expected to return true");
		ASSERT_FALSE(tree.find(3).valid(), "expected to have the element removed")
	}

	// LL
	{
		ASSERT_TRUE(tree.remove(6), "expected to return true");
		ASSERT_FALSE(tree.find(6).valid(), "expected to have the element removed")
		ASSERT_EQUALS(2, tree.get_root()->get_obj(), "expected to have the tree re-balanced")
	}

	// RR
	{
		tree.insert(data_to_be_inserted[7 - 1]);

		ASSERT_TRUE(tree.remove(1), "expected to return true");
		ASSERT_FALSE(tree.find(1).valid(), "expected to have the element removed")
		ASSERT_EQUALS(4, tree.get_root()->get_obj(), "expected to have the tree re-balanced")
	}

	// LR
	{
		tree.insert(new int(3));

		ASSERT_TRUE(tree.remove(7), "expected to return true");
		ASSERT_FALSE(tree.find(7).valid(), "expected to have the element removed")
		ASSERT_EQUALS(3, tree.get_root()->get_obj(), "expected to have the tree re-balanced")
	}

	// RL
	{
		// release 4
		tree.get_root()->release_right();
		tree.insert(data_to_be_inserted[5 - 1]);
		tree.insert(data_to_be_inserted[4 - 1]);

		ASSERT_TRUE(tree.remove(2), "expected to return true");
		ASSERT_FALSE(tree.find(2).valid(), "expected to have the element removed")
		ASSERT_EQUALS(4, tree.get_root()->get_obj(), "expected to have the tree re-balanced")
	}

ENDDEF_TEST_CASE

void test_avl_tree()
{
	ghl::test_unit unit
	{
		{
			&test_avl_tree_check_imbalance_on_path,
			&test_avl_tree_insert,
			&test_avl_tree_remove
		},
		"tests for avl tree"
	};

	unit.execute();

	std::cout << unit.get_msg() << "\n";
}