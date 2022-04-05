/*
* This file contains the definition of queue data structrues
*/

#pragma once

#include "vector.h"
#include "list.h"

namespace ghl
{
	template <typename T>
	class queue
	{

	private:
		// the actual implementation
		std::unique_ptr<T> imp;
	};

	/*
	* ADT priority queue
	* 
	* A queue whose elements are in ascending order.
	* 
	* Invariant:
	* All elements appear in ascending order if G is true (by default)
	* or in descending order otherwise
	* 
	* Requirements:
	* 1. T supports operator<= which imposes a total ordering on objs of T
	* 2. T is constructable and destructable, and in addition copyable
	*/
	template <typename T, bool A = true>
	class priority_queue
	{
		// for unit tests
		friend class priority_queue_tester;
	public:
		priority_queue() {};
		priority_queue(const std::initializer_list<T>& in_list)
		{
			for (const T& o : in_list)
			{
				insert(o);
			}
		}
		~priority_queue() {}

	public:
		size_t size() const { return lst.size(); }
		bool empty() const { return lst.empty(); }

		/*
		* Peeks the smallest one
		*/
		T& peek() const { return lst.front(); }

		/*
		* Gets and removes the smallest one
		*/
		T pop() { T res(lst.front()); lst.remove_front(); return res; }
		// Performs an insertion according to the order,
		// like what we do in the insertion sort ( O(n) )
		void push(const T& obj)
		{
			bool bInserted = false;
			for (auto i = lst.begin(); i != lst.end(); ++i)
			{
				if (obj <= *i) // the first one >= obj
				{
					bInserted = true;
					lst.insert(i, obj);
					break;
				}
			}

			if (!bInserted) // the element > all elements
			{
				lst.insert_back(obj);
			}
		}

	private:
		ghl::list<T> lst;
	};

	template <typename T>
	class priority_queue<T, false>
	{
		// for unit tests
		friend class priority_queue_tester;
	public:
		priority_queue() {};
		priority_queue(const std::initializer_list<T>& in_list)
		{
			for (const T& o : in_list)
			{
				insert(o);
			}
		}
		~priority_queue() {}

	public:
		size_t size() const { return lst.size(); }
		bool empty() const { return lst.empty(); }

		/*
		* Peeks the largest one
		*/
		T& peek() const { return lst.front(); }

		/*
		* Gets and removes the largest one
		*/
		T pop() { T res(lst.front()); lst.remove_front(); return res; }
		// Performs an insertion according to the order,
		// like what we do in the insertion sort ( O(n) )
		void push(const T& obj)
		{
			bool bInserted = false;
			for (auto i = lst.begin(); i != lst.end(); ++i)
			{
				if (!(obj <= *i)) // the first one <= obj (the condition equals to obj >= i since the order is a total order)
				{
					bInserted = true;
					lst.insert(i, obj);
					break;
				}
			}

			if (!bInserted) // the element < all elements
			{
				lst.insert_back(obj);
			}
		}

	private:
		ghl::list<T> lst;
	};
}