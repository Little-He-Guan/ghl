#include "../data_structures/queue.h"
#include "../unit_test/test_unit.h"

#include <iostream>

namespace ghl
{
	class priority_queue_tester
	{
	public:
		template <typename Q>
		static auto& lst(Q& queue) { return queue.lst; }
	};
}

DEFINE_TEST_CASE(test_priority_queue)

	// empty queue
	{
		ghl::priority_queue<int> q;

		ASSERT_TRUE(q.empty(), "expected to be empty when created with no arguments")
	}

	// push and pop (see if elements are added and are placed in ascending order; and if pop removes and returns in the correct order)
	{
		// asc order queue
		{
			ghl::priority_queue<int> q;

			auto& lst = ghl::priority_queue_tester::lst(q);
		
			// first insert in ascending order
			q.push(0);
			q.push(2);
			q.push(3);
			q.push(6);

			ASSERT_EQUALS(4, q.size(), "expected to have 4 elements added")
			auto bg = lst.cbegin();
			ASSERT_EQUALS(0, *bg, "expected to have the order of the elements right")
			ASSERT_EQUALS(2, *(bg + 1), "expected to have the order of the elements right")
			ASSERT_EQUALS(3, *(bg + 2), "expected to have the order of the elements right")
			ASSERT_EQUALS(6, *(bg + 3), "expected to have the order of the elements right")

			// then insert in descending order
			q.push(30);
			q.push(22);
			q.push(21);
			q.push(16);

			ASSERT_EQUALS(8, q.size(), "expected to have 4 elements added")
			bg = lst.cbegin();
			ASSERT_EQUALS(16, *(bg + 4), "expected to have the order of the elements right")
			ASSERT_EQUALS(21, *(bg + 5), "expected to have the order of the elements right")
			ASSERT_EQUALS(22, *(bg + 6), "expected to have the order of the elements right")
			ASSERT_EQUALS(30, *(bg + 7), "expected to have the order of the elements right")

			// now insert elements that are in between previous eles
			q.push(18);
			q.push(1);
			q.push(5);
			q.push(25);

			ASSERT_EQUALS(12, q.size(), "expected to have 4 elements added")
			bg = lst.cbegin();
			ASSERT_EQUALS(0, *bg, "expected to have the order of the elements right")
			ASSERT_EQUALS(1, *(bg + 1), "expected to have the order of the elements right")
			ASSERT_EQUALS(2, *(bg + 2), "expected to have the order of the elements right")
			ASSERT_EQUALS(3, *(bg + 3), "expected to have the order of the elements right")
			ASSERT_EQUALS(5, *(bg + 4), "expected to have the order of the elements right")
			ASSERT_EQUALS(6, *(bg + 5), "expected to have the order of the elements right")
			ASSERT_EQUALS(16, *(bg + 6), "expected to have the order of the elements right")
			ASSERT_EQUALS(18, *(bg + 7), "expected to have the order of the elements right")
			ASSERT_EQUALS(21, *(bg + 8), "expected to have the order of the elements right")
			ASSERT_EQUALS(22, *(bg + 9), "expected to have the order of the elements right")
			ASSERT_EQUALS(25, *(bg + 10), "expected to have the order of the elements right")
			ASSERT_EQUALS(30, *(bg + 11), "expected to have the order of the elements right")

			ASSERT_EQUALS(0, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(1, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(2, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(3, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(5, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(6, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(16, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(18, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(21, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(22, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(25, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(30, q.pop(), "expected to have the order of the elements right")

			ASSERT_TRUE(q.empty(), "expected to have none now")
		}

		// desc order queue
		{
			ghl::priority_queue<int, false> q;

			auto& lst = ghl::priority_queue_tester::lst(q);
		
			// first insert in descending order
			q.push(30);
			q.push(22);
			q.push(21);
			q.push(16);
			ASSERT_EQUALS(4, q.size(), "expected to have 4 elements added")
			auto bg = lst.cbegin();
			ASSERT_EQUALS(30, *(bg + 0), "expected to have the order of the elements right")
			ASSERT_EQUALS(22, *(bg + 1), "expected to have the order of the elements right")
			ASSERT_EQUALS(21, *(bg + 2), "expected to have the order of the elements right")
			ASSERT_EQUALS(16, *(bg + 3), "expected to have the order of the elements right")

			// then insert in ascending order
			q.push(0);
			q.push(2);
			q.push(3);
			q.push(6);
			ASSERT_EQUALS(8, q.size(), "expected to have 4 elements added")
			bg = lst.cbegin();
			ASSERT_EQUALS(6, *(bg + 4), "expected to have the order of the elements right")
			ASSERT_EQUALS(3, *(bg + 5), "expected to have the order of the elements right")
			ASSERT_EQUALS(2, *(bg + 6), "expected to have the order of the elements right")
			ASSERT_EQUALS(0, *(bg + 7), "expected to have the order of the elements right")

			// now insert elements that are in between previous eles
			q.push(25);
			q.push(5);
			q.push(1);
			q.push(18);

			ASSERT_EQUALS(12, q.size(), "expected to have 4 elements added")
			bg = lst.cbegin();
			ASSERT_EQUALS(30, *bg, "expected to have the order of the elements right")
			ASSERT_EQUALS(25, *(bg + 1), "expected to have the order of the elements right")
			ASSERT_EQUALS(22, *(bg + 2), "expected to have the order of the elements right")
			ASSERT_EQUALS(21, *(bg + 3), "expected to have the order of the elements right")
			ASSERT_EQUALS(18, *(bg + 4), "expected to have the order of the elements right")
			ASSERT_EQUALS(16, *(bg + 5), "expected to have the order of the elements right")
			ASSERT_EQUALS(6, *(bg + 6), "expected to have the order of the elements right")
			ASSERT_EQUALS(5, *(bg + 7), "expected to have the order of the elements right")
			ASSERT_EQUALS(3, *(bg + 8), "expected to have the order of the elements right")
			ASSERT_EQUALS(2, *(bg + 9), "expected to have the order of the elements right")
			ASSERT_EQUALS(1, *(bg + 10), "expected to have the order of the elements right")
			ASSERT_EQUALS(0, *(bg + 11), "expected to have the order of the elements right")

			ASSERT_EQUALS(30, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(25, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(22, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(21, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(18, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(16, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(6, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(5, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(3, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(2, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(1, q.pop(), "expected to have the order of the elements right")
			ASSERT_EQUALS(0, q.pop(), "expected to have the order of the elements right")

			ASSERT_TRUE(q.empty(), "expected to have none now")
		}
	}

ENDDEF_TEST_CASE

void test_queue()
{
	ghl::test_unit pri_que
	{
		{
			&test_priority_queue
		},
		"tests for priority queues"
	};

	pri_que.execute();
	std::cout << pri_que.get_msg() << "\n";
}