#include "../data_structures/vector.h"
#include "../unit_test/test_unit.h"
#include "../unit_test/example_test_classes.h"

#include <iostream>

namespace ghl
{
	// used to access private members
	class vector_tester
	{
	public:
		template <typename T>
		static auto get_start(ghl::vector<T>& v) { return v.mp_start; }
	};
}

#pragma region vector_constructor_test

DEFINE_TEST_CASE(test_vector_constructor_size)

	// pass a 0
	ghl::vector<int> v0(0);

	ASSERT_TRUE(v0.check_rep(), "expected to have the invariant held")
	
	ASSERT_TRUE(nullptr != ghl::vector_tester::get_start(v0), "expected to get an allocated space");
	ASSERT_EQUALS(1, v0.capacity(), "expected to get a 1 capacity")
	ASSERT_EQUALS(0, v0.size(), "expected to get a 0 size")

	// pass a non-zero value
	ghl::vector<int> vsome(3);
	ASSERT_TRUE(nullptr != ghl::vector_tester::get_start(vsome), "expected to get an allocated space");
	ASSERT_EQUALS(3, vsome.capacity(), "expected to get a 3 capacity")
	ASSERT_EQUALS(0, vsome.size(), "expected to get a 0 size")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_vector_constructor_list)

	// empty list
	ghl::vector<int> v0(std::initializer_list<int>{});

	ASSERT_TRUE(v0.check_rep(), "expected to have the invariant held")

	ASSERT_TRUE(nullptr != ghl::vector_tester::get_start(v0), "expected to get an allocated space");
	ASSERT_EQUALS(1, v0.capacity(), "expected to get a 1 capacity")
	ASSERT_EQUALS(0, v0.size(), "expected to get a 0 size")

	// pass a non-empty list
	ghl::vector<int> vsome{ 1, 2, 3 };
	ASSERT_TRUE(nullptr != ghl::vector_tester::get_start(vsome), "expected to get an allocated space");
	ASSERT_EQUALS(3, vsome.capacity(), "expected to get a 3 capacity")
	ASSERT_EQUALS(3, vsome.size(), "expected to get a 3 size")

	ASSERT_EQUALS(1, vsome[0], "expected to get 1 for the first element")
	ASSERT_EQUALS(2, vsome[1], "expected to get 2 for the second element")
	ASSERT_EQUALS(3, vsome[2], "expected to get 3 for the third element")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_vector_constructor_iter)

	// begin = end
	ghl::vector<int> v0((int*)nullptr, (int*)nullptr);

	ASSERT_TRUE(v0.check_rep(), "expected to have the invariant held")

	ASSERT_TRUE(nullptr != ghl::vector_tester::get_start(v0), "expected to get an allocated space");
	ASSERT_EQUALS(1, v0.capacity(), "expected to get a 1 capacity")
	ASSERT_EQUALS(0, v0.size(), "expected to get a 0 size")

	// begin > end
	ghl::vector<int> v1((int*)333000, (int*)nullptr);

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_TRUE(nullptr != ghl::vector_tester::get_start(v1), "expected to get an allocated space");
	ASSERT_EQUALS(1, v1.capacity(), "expected to get a 1 capacity")
	ASSERT_EQUALS(0, v1.size(), "expected to get a 0 size")

	// end > begin
	ghl::vector<int> vtemp{ 1, 2, 3 };
	ghl::vector<int> vsome(vtemp.begin(), vtemp.end());
	ASSERT_TRUE(nullptr != ghl::vector_tester::get_start(vsome), "expected to get an allocated space");
	ASSERT_EQUALS(3, vsome.capacity(), "expected to get a 3 capacity")
	ASSERT_EQUALS(3, vsome.size(), "expected to get a 3 size")

	ASSERT_EQUALS(1, vsome[0], "expected to get 1 for the first element")
	ASSERT_EQUALS(2, vsome[1], "expected to get 2 for the second element")
	ASSERT_EQUALS(3, vsome[2], "expected to get 3 for the third element")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_vector_copy_constructor)

	ghl::vector<int> v0{ 4,5,6 };
	ghl::vector<int> v1(v0);

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(v0.size(), v1.size(), "expected to have the sizes equal")
	ASSERT_EQUALS(v0.capacity(), v1.capacity(), "expected to have the capacities equal")

	ASSERT_EQUALS(4, v1[0], "expected to get 4 for the first element")
	ASSERT_EQUALS(5, v1[1], "expected to get 5 for the second element")
	ASSERT_EQUALS(6, v1[2], "expected to get 6 for the third element")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_vector_move_constructor)

	ghl::vector<int> v0{ 4,5,6 };
	ghl::vector<int> v1(std::move(v0));

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(3, v1.size(), "expected to have the sizes equal")
	ASSERT_EQUALS(3, v1.capacity(), "expected to have the capacities equal")

	ASSERT_EQUALS(4, v1[0], "expected to get 4 for the first element")
	ASSERT_EQUALS(5, v1[1], "expected to get 5 for the second element")
	ASSERT_EQUALS(6, v1[2], "expected to get 6 for the third element")

	ASSERT_EQUALS(0, v0.size(), "expected to have the moved obj cleared")
	ASSERT_EQUALS(0, v0.capacity(), "expected to have the moved obj cleared")
	ASSERT_EQUALS(nullptr, ghl::vector_tester::get_start(v0), "expected to have the moved obj cleared")

ENDDEF_TEST_CASE

#pragma endregion

#pragma region vector_assignments_test

DEFINE_TEST_CASE(test_vector_copy_assignment)

	// first test when we don't need a larger buffer
	ghl::vector<int> v0{ 4,5,6 };
	ghl::vector<int> v1{ 1,2 };
	v1 = v0;

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(v0.size(), v1.size(), "expected to have the sizes equal")
	ASSERT_EQUALS(v0.capacity(), v1.capacity(), "expected to have the capacities equal")

	ASSERT_EQUALS(4, v1[0], "expected to get 4 for the first element")
	ASSERT_EQUALS(5, v1[1], "expected to get 5 for the second element")
	ASSERT_EQUALS(6, v1[2], "expected to get 6 for the third element")

	// now test when we need a larger buffer
	ghl::vector<int> v2{ 9,9,9,9 };
	v2 = v1;

	ASSERT_TRUE(v2.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(v0.size(), v2.size(), "expected to have the sizes equal")
	ASSERT_EQUALS(4, v2.capacity(), "expected to have the capacities unchanged")

	ASSERT_EQUALS(4, v2[0], "expected to get 4 for the first element")
	ASSERT_EQUALS(5, v2[1], "expected to get 5 for the second element")
	ASSERT_EQUALS(6, v2[2], "expected to get 6 for the third element")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_vector_move_assignment)

	ghl::vector<int> v0{ 4,5,6 };
	ghl::vector<int> v1{ 1,2 };
	v1 = std::move(v0);

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(3, v1.size(), "expected to have the sizes equal")
	ASSERT_EQUALS(3, v1.capacity(), "expected to have the capacities equal")

	ASSERT_EQUALS(4, v1[0], "expected to get 4 for the first element")
	ASSERT_EQUALS(5, v1[1], "expected to get 5 for the second element")
	ASSERT_EQUALS(6, v1[2], "expected to get 6 for the third element")

	ASSERT_EQUALS(0, v0.size(), "expected to have the moved obj cleared")
	ASSERT_EQUALS(0, v0.capacity(), "expected to have the moved obj cleared")
	ASSERT_EQUALS(nullptr, ghl::vector_tester::get_start(v0), "expected to have the moved obj cleared")

ENDDEF_TEST_CASE

#pragma endregion

#pragma region vector_operation_test

DEFINE_TEST_CASE(test_vector_resize)

	ghl::vector<int> v1 {1,2,3};

	// first we try to resize it to a smaller size (expect to no nothing)
	v1.resize(2);

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(3, v1.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(3, v1.capacity(), "expected to have the capacity unchanged")

	ASSERT_EQUALS(1, v1[0], "expected to have the elements unchanged")
	ASSERT_EQUALS(2, v1[1], "expected to have the elements unchanged")
	ASSERT_EQUALS(3, v1[2], "expected to have the elements unchanged")

	// now we try to resize it to a larger size
	v1.resize(5);

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(3, v1.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(5, v1.capacity(), "expected to have the capacity changed")

	ASSERT_EQUALS(1, v1[0], "expected to have the elements unchanged")
	ASSERT_EQUALS(2, v1[1], "expected to have the elements unchanged")
	ASSERT_EQUALS(3, v1[2], "expected to have the elements unchanged")

	// vector of only copyable elements
	ghl::vector<ghl::test_class_copy> vc{ ghl::test_class_copy{1},ghl::test_class_copy{2} };
	// should compile
	vc.resize(5);

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_vector_push_back)

	ghl::vector<int> v1(2);

	// first we try to push_back to an vector that has enough capacity
	v1.push_back(5);

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(1, v1.size(), "expected to have the size increased")
	ASSERT_EQUALS(2, v1.capacity(), "expected to have the capacity unchanged")

	ASSERT_EQUALS(5, v1[0], "expected to have the added element")

	// now we try to push_back until it has to increase its capacity
	v1.push_back(1);
	v1.push_back(3);

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(3, v1.size(), "expected to have the size increased")
	ASSERT_EQUALS(3, v1.capacity(), "expected to have the capacity increased")

	ASSERT_EQUALS(5, v1[0], "expected to have the existing element unchanged")
	ASSERT_EQUALS(1, v1[1], "expected to have the added element")
	ASSERT_EQUALS(3, v1[2], "expected to have the added element")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_vector_emplace_back)

	ghl::vector<ghl::test_class_copy_move> v1(2);

	// first we try to emplace_back to an vector that has enough capacity
	v1.emplace_back(5);

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(1, v1.size(), "expected to have the size increased")
	ASSERT_EQUALS(2, v1.capacity(), "expected to have the capacity unchanged")

	ASSERT_EQUALS(5, v1[0].val, "expected to have the added element")

	ASSERT_TRUE(v1[0].b_constructed_normally, "the element should not be copy or move constructed")

	// now we try to emplace until it has to increase its capacity
	v1.emplace_back(1);
	v1.emplace_back(3);

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(3, v1.size(), "expected to have the size increased")
	ASSERT_EQUALS(3, v1.capacity(), "expected to have the capacity increased")

	ASSERT_EQUALS(5, v1[0].val, "expected to have the existing element unchanged")
	ASSERT_EQUALS(1, v1[1].val, "expected to have the added element")
	ASSERT_EQUALS(3, v1[2].val, "expected to have the added element")

	// we cannot make such an assertion about v1[1] because after the buffer is reallocated, it has to be moved to the new buffer
	//ASSERT_TRUE(v1[1].b_constructed_normally, "the element should not be copy or move constructed")
	ASSERT_TRUE(v1[2].b_constructed_normally, "the element should not be copy or move constructed")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_vector_remove_back)

	ghl::vector<int> v1(2);

	// first we try to remove_back on an empty vector (expect to do nothing)
	v1.remove_back();

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(0, v1.size(), "expected to have the size unchanged")
	ASSERT_EQUALS(2, v1.capacity(), "expected to have the capacity unchanged")

	// now we try to remove an added element (requires push_back to function properly)
	v1.push_back(1);
	v1.push_back(3);

	v1.remove_back();

	ASSERT_TRUE(v1.check_rep(), "expected to have the invariant held")

	ASSERT_EQUALS(1, v1.size(), "expected to have the size decreased")
	ASSERT_EQUALS(2, v1.capacity(), "expected to have the capacity unchanged")

	ASSERT_EQUALS(1, v1[0], "expected to have the unremoved element unchanged")

ENDDEF_TEST_CASE

#pragma endregion

void test_vector()
{
	ghl::test_unit ctor_unit
	{ 
		{
			&test_vector_constructor_size, 
			&test_vector_constructor_list,
			&test_vector_constructor_iter,
			&test_vector_copy_constructor,
			&test_vector_move_constructor,
		}, 
		"tests for constructors of vector" 
	};

	ghl::test_unit ass_unit
	{
		{
			&test_vector_copy_assignment,
			&test_vector_move_assignment,
		},
		"tests for assignments of vector"
	};

	ghl::test_unit oper_unit
	{
		{
			&test_vector_resize,
			&test_vector_push_back,
			&test_vector_emplace_back,
			&test_vector_remove_back
		},
		"tests for operations of vector"
	};

	ctor_unit.execute();
	ass_unit.execute();
	oper_unit.execute();

	std::cout
		<< ctor_unit.get_msg() << "\n"
		<< ass_unit.get_msg() << "\n"
		<< oper_unit.get_msg() << "\n"
		<< std::endl;
}