#include "set_test.h"

#include "../data_structures/set.h"

#include <iostream>

void test_tree_set()
{
	ghl::test_unit unit
	{
		{
			&test_set_add<ghl::tree_set<int>>,
			&test_set_remove<ghl::tree_set<int>>,
			&test_set_any_element<ghl::tree_set<int>>
		},
		"tests for tree set"
	};

	unit.execute();

	std::cout << unit.get_msg() << "\n";
}