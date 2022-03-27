#include "../data_structures/list.h"
#include "../unit_test/test_unit.h"

#include "../data_structures/vector.h"
#include "../unit_test/example_test_classes.h"

#include <iostream>

namespace ghl
{
	// used to access private members
	class list_tester
	{
	public:
		template <typename T>
		static auto head(ghl::list<T>& l) { return l.head; }
		template <typename T>
		static auto tail(ghl::list<T>& l) { return l.tail; }

		template <typename T>
		using node = typename ghl::list<T>::node;
	};

	/*
	* A test class that can in addition tell if an instance is destructed
	*/
	class test_class_copy_move_dest : public test_class_copy_move
	{
	public:
		test_class_copy_move_dest(int ind, int val) : index(ind), test_class_copy_move(val) {};
		~test_class_copy_move_dest() { destructed[index] = true; }

	public:
		int index;

		static bool destructed[10];
		static void refresh_destructed() {	for (auto& b : destructed) { b = false; } }
	};

	bool test_class_copy_move_dest::destructed[10] = {0,0,0,0,0,0,0,0,0,0};
}

#pragma region test_list_ctor_dtor

DEFINE_TEST_CASE(test_list_ctor_default)

	ghl::list<int> l;

	ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(nullptr, ghl::list_tester::head(l), "expected to have a nullptr head")
	ASSERT_EQUALS(nullptr, ghl::list_tester::tail(l).lock(), "expected to have a nullptr tail")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_ctor_list)

	// empty list
	{
		ghl::list<int> l{std::initializer_list<int>()};

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		ASSERT_EQUALS(nullptr, ghl::list_tester::head(l), "expected to have a nullptr head")
		ASSERT_EQUALS(nullptr, ghl::list_tester::tail(l).lock(), "expected to have a nullptr tail")
	}

	// non-empty list
	{
		ghl::list<int> l{ 1,2,3 };

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		ASSERT_TRUE(nullptr != ghl::list_tester::head(l), "expected to have a non-nullptr head")
		ASSERT_TRUE(nullptr != ghl::list_tester::tail(l).lock(), "expected to have a non-nullptr tail")

		ASSERT_EQUALS(1, ghl::list_tester::head(l)->get_obj(), "expected to have the elements")
		ASSERT_EQUALS(2, ghl::list_tester::head(l)->next->get_obj(), "expected to have the elements")
		ASSERT_EQUALS(3, ghl::list_tester::head(l)->next->next->get_obj(), "expected to have the elements")
		ASSERT_EQUALS(3, ghl::list_tester::tail(l).lock()->get_obj(), "expected to have tail to be the last ele")
	}	

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_ctor_iter)

	// empty list
	{
		ghl::vector<int> v;
		ghl::list<int> l{v.begin(),v.end()};

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		ASSERT_EQUALS(nullptr, ghl::list_tester::head(l), "expected to have a nullptr head")
		ASSERT_EQUALS(nullptr, ghl::list_tester::tail(l).lock(), "expected to have a nullptr tail")
	}

	// non-empty list
	{
		ghl::vector<int> v{1,2,3};
		ghl::list<int> l{ v.begin(),v.end() };

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		ASSERT_TRUE(nullptr != ghl::list_tester::head(l), "expected to have a non-nullptr head")
		ASSERT_TRUE(nullptr != ghl::list_tester::tail(l).lock(), "expected to have a non-nullptr tail")

		ASSERT_EQUALS(1, ghl::list_tester::head(l)->get_obj(), "expected to have the elements")
		ASSERT_EQUALS(2, ghl::list_tester::head(l)->next->get_obj(), "expected to have the elements")
		ASSERT_EQUALS(3, ghl::list_tester::head(l)->next->next->get_obj(), "expected to have the elements")
		ASSERT_EQUALS(3, ghl::list_tester::tail(l).lock()->get_obj(), "expected to have tail to be the last ele")
	}	

ENDDEF_TEST_CASE

// ensures that no resource leaks (we use shared_ptr in a bidirectional list, which may cause circular ownership)
DEFINE_TEST_CASE(test_list_dtor)

	std::shared_ptr<ghl::list_tester::node<ghl::test_class_copy_move_dest>> p;
	{
		// dtor will be called at the end of the block
		ghl::list<ghl::test_class_copy_move_dest> l{ ghl::test_class_copy_move_dest{0,1}, ghl::test_class_copy_move_dest{ 1,2 } };
		// refresh now because the destruction of std::init_list destroys the objs
		ghl::test_class_copy_move_dest::refresh_destructed();		
	}

	ASSERT_TRUE(ghl::test_class_copy_move_dest::destructed[0], "expected to have the element destructed")
	ASSERT_TRUE(ghl::test_class_copy_move_dest::destructed[1], "expected to have the element destructed")

ENDDEF_TEST_CASE

#pragma endregion

#pragma region test_list_iter

DEFINE_TEST_CASE(test_list_iter_inc_prefix)

	ghl::list<int> l{ 1,2,3 };

	auto i = l.begin();
	auto ret_val = ++i;

	ASSERT_EQUALS(2, *i, "expected to advance i")
	ASSERT_EQUALS(2, *ret_val, "expected to return the current val")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_iter_inc_postfix)

	ghl::list<int> l{ 1,2,3 };

	auto i = l.begin();
	auto ret_val = i++;

	ASSERT_EQUALS(2, *i, "expected to advance i")
	ASSERT_EQUALS(1, *ret_val, "expected to return the prev val")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_iter_dec_prefix)

	ghl::list<int> l{ 1,2,3 };

	auto i = ghl::list<int>::iterator(ghl::list_tester::tail(l).lock());
	auto ret_val = --i;

	ASSERT_EQUALS(2, *i, "expected to make i go back")
	ASSERT_EQUALS(2, *ret_val, "expected to return the current val")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_iter_dec_postfix)

	ghl::list<int> l{ 1,2,3 };

	auto i = ghl::list<int>::iterator(ghl::list_tester::tail(l).lock());
	auto ret_val = i--;

	ASSERT_EQUALS(2, *i, "expected to make i go back")
	ASSERT_EQUALS(3, *ret_val, "expected to return the prev val")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_iter_addition)

	ghl::list<int> l{ 2,3,4,5,6 };

	auto i = l.begin() + 1;

	ASSERT_EQUALS(3, *i, "expected to have the correct val")
	ASSERT_EQUALS(6, *(i+3), "expected to have the correct val")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_iter_subtraction)

	ghl::list<int> l{ 2,3,4,5,6 };

	auto i = l.end();

	ASSERT_EQUALS(6, *(i-1), "expected to have the correct val")
	ASSERT_EQUALS(4, *(i-3), "expected to have the correct val")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_iter_dist)

	ghl::list<int> l{2,3,4,5,6};

	auto e = l.end();
	auto b = l.begin();

	ASSERT_EQUALS(5, e - b, "expected to have the dist correct")
	ASSERT_EQUALS(3, e-(b+2), "expected to have the dist correct")
	ASSERT_EQUALS(4, (e-1) - b, "expected to have the dist correct")

	ghl::list<int> l1;
	l1.emplace_back(1); l1.emplace_back(2);

	ASSERT_EQUALS(2, l1.size(), "expected to have the dist correct")

ENDDEF_TEST_CASE

// ensures that an iter's existance prevents an obj from being destructed
DEFINE_TEST_CASE(test_list_iter_own_obj)

	ghl::list<int> l{ 2,3,4,5,6 };

	ghl::test_class_copy_move_dest::refresh_destructed();

	{
		ghl::list<ghl::test_class_copy_move_dest>::iterator i(nullptr);

		{
			ghl::list<ghl::test_class_copy_move_dest> l{ {0,1},{1,2} };
			// refresh now because the destruction of std::init_list destroys the objs
			ghl::test_class_copy_move_dest::refresh_destructed();
			i = l.begin();
		}

		ASSERT_FALSE(ghl::test_class_copy_move_dest::destructed[0], "expected to hold the obj")
		// we cannot assert that other objs are released now, because they are owned by their prev, and are thus indirectly owned by some iter present
		//ASSERT_TRUE(ghl::test_class_copy_move_dest::destructed[1], "expected to release the other obj(s)")
	}

	ASSERT_TRUE(ghl::test_class_copy_move_dest::destructed[0], "when iter is destroyed, so should all objs be")
	ASSERT_TRUE(ghl::test_class_copy_move_dest::destructed[1], "when iter is destroyed, so should all objs be")

ENDDEF_TEST_CASE

#pragma endregion

#pragma region test_list_operations

// since that insert_front, insert_back ... and similar methods are all direct calls to other complex methods,
// we only test the complex methods here

DEFINE_TEST_CASE(test_list_insert)

	// insert on empty list
	{
		ghl::list<int> l;
		auto i = l.insert_front(3);

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		ASSERT_FALSE(l.empty(), "the list should not be empty now")
		ASSERT_EQUALS(3, *i, "expected to return the inserted element")
	}

	// insert on non-empty list
	{
		ghl::list<int> l{ 1,2,3 };
		auto i = l.begin() + 1;
		auto ret = l.insert(i, 5);

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		ASSERT_EQUALS(4, l.size(), "the list should grow")
		ASSERT_EQUALS(5, *ret, "expected to return the inserted element")
		ASSERT_EQUALS(2, *(ret+1), "expected to insert before pos")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_emplace)

	// emplace on empty list
	{
		ghl::list<ghl::test_class_copy_move> l;
		auto i1 = l.emplace_back(1);

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		ASSERT_FALSE(l.empty(), "the list should not be empty now")
		ASSERT_EQUALS(1, l.size(), "expected to have the size changed to 1")

		ASSERT_EQUALS(1, i1->val, "expected to return the inserted element")
		ASSERT_TRUE(i1->b_constructed_normally, "expected to have the obj directly constructed")
	}

	// insert on non-empty list
	{
		ghl::list<ghl::test_class_copy_move> l{ ghl::test_class_copy_move(2),ghl::test_class_copy_move(3),ghl::test_class_copy_move(4) };
		auto i = l.begin() + 1;
		auto ret = l.emplace(i,5);

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		ASSERT_EQUALS(4, l.size(), "the list should grow")
		ASSERT_EQUALS(5, ret->val, "expected to return the inserted element")
		ASSERT_TRUE(ret->b_constructed_normally, "expected to have the obj directly constructed")
		ASSERT_EQUALS(3, (ret + 1)->val, "expected to insert before pos")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_list_remove)

	// on empty list
	{
		ghl::list<int> l;

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		l.remove_front();
		ASSERT_TRUE(l.empty(), "expected to do nothing")

		l.remove_back();
		ASSERT_TRUE(l.empty(), "expected to do nothing")
	}

	// on non-empty list
	{
		ghl::list<int> l{ 2,3,4,5 };

		ASSERT_TRUE(l.check_rep(), "expected to have the invariant held")

		auto i = l.remove_back();
		ASSERT_EQUALS(3, l.size(), "the list should shrink")
		ASSERT_EQUALS(2,*l.begin(),"expected to have the other objs unchanged")
		ASSERT_EQUALS(3, *(l.begin()+1), "expected to have the other objs unchanged")
		ASSERT_EQUALS(4, *(l.begin()+2), "expected to have the other objs unchanged")
		ASSERT_EQUALS(l.end(), i, "expected to get end() after removing back")

		i = l.remove(l.begin()+1);
		ASSERT_EQUALS(2, l.size(), "the list should shrink")
		ASSERT_EQUALS(2, *l.begin(), "expected to have the other objs unchanged")
		ASSERT_EQUALS(4, *(l.begin() + 1), "expected to have the other objs unchanged")
		ASSERT_EQUALS(*i ,4, "expected to get the next ele")
	}

ENDDEF_TEST_CASE

#pragma endregion

DEFINE_TEST_CASE(test_list_prev_bugs) // make sure prev bugs do not happen again

	// first bug: insert/emplace before end or before head for an non-empty set
	//				does not update head or tail properly
	{
		ghl::list<int> l{ 1,2 };

		l.insert_front(0);
		ASSERT_EQUALS(3, l.size(), "expected to increase the size")
		ASSERT_EQUALS(0, ghl::list_tester::head(l)->get_obj(), "expected to have head point to the newly added obj")

		l.insert_back(3);
		ASSERT_EQUALS(4, l.size(), "expected to increase the size")
		ASSERT_EQUALS(3, ghl::list_tester::tail(l).lock()->get_obj(), "expected to have head point to the newly added obj")

		l.emplace_front(-1);
		ASSERT_EQUALS(5, l.size(), "expected to increase the size")
		ASSERT_EQUALS(-1, ghl::list_tester::head(l)->get_obj(), "expected to have head point to the newly added obj")

		l.emplace_back(4);
		ASSERT_EQUALS(6, l.size(), "expected to increase the size")
		ASSERT_EQUALS(4, ghl::list_tester::tail(l).lock()->get_obj(), "expected to have head point to the newly added obj")
	}

	// second bug removing head or tail does not update it properly
	{
		ghl::list<int> l{ 1,2,3 };

		l.remove_back();
		ASSERT_EQUALS(2, l.size(), "expected to decrease the size")
		ASSERT_EQUALS(2, ghl::list_tester::tail(l).lock()->get_obj(), "expected to have tail updated")

		l.remove_front();
		ASSERT_EQUALS(1, l.size(), "expected to decrease the size")
		ASSERT_EQUALS(2, ghl::list_tester::head(l)->get_obj(), "expected to have head updated")

		// now head = tail, after removing l should be empty
		l.remove_back();
		ASSERT_TRUE(l.empty(), "expected to be empty")
	}

ENDDEF_TEST_CASE

void test_list()
{
	ghl::test_unit ctor_and_dtor
	{
		{
			&test_list_ctor_default,
			&test_list_ctor_list,
			&test_list_ctor_iter,
			&test_list_dtor
		},
		"test cases for list ctors and dtor"
	};

	ghl::test_unit iter
	{
		{
			&test_list_iter_inc_prefix,
			&test_list_iter_inc_postfix,
			&test_list_iter_dec_prefix,
			&test_list_iter_dec_postfix,
			&test_list_iter_addition,
			&test_list_iter_subtraction,
			&test_list_iter_dist,
			&test_list_iter_own_obj
		},
		"test cases for list iters"
	};

	ghl::test_unit oper
	{
		{
			&test_list_insert,
			&test_list_emplace,
			&test_list_remove
		},
		"test cases for list operations"
	};

	ghl::test_unit prev_bugs
	{
		{
			&test_list_prev_bugs
		},
		"test cases for previous bugs"
	};

	ctor_and_dtor.execute();
	std::cout << ctor_and_dtor.get_msg() << "\n";

	iter.execute();
	std::cout << iter.get_msg() << "\n";

	oper.execute();
	std::cout << oper.get_msg() << "\n";

	prev_bugs.execute();
	std::cout << prev_bugs.get_msg() << "\n";
}