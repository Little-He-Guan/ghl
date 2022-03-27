/*
* Some templates may hold compound types like a class
*
* The classes in this header define in a simple way different basic operations that a real class may support
* so that the user may test the behaviors or their templates
*
* All members of these classes are public for test purposes
*/

#pragma once

namespace ghl
{
	/*
	* A Cpp17CopyConstructible, Cpp17MoveConstructible, and Cpp17Destructible type
	*/
	class test_class_copy_move
	{
	public:
		explicit test_class_copy_move(int value) :
			val(value),
			b_constructed_normally(true)
		{}

		test_class_copy_move(const test_class_copy_move& other) noexcept :
			val(other.val),
			b_copy_constructed(true)
		{}
		test_class_copy_move(test_class_copy_move&& other) noexcept :
			val(other.val),
			b_move_constructed(true)
		{}

		~test_class_copy_move() noexcept {}

	public:
		int val;
		// true iff constructed not by copy and move ctors
		bool b_constructed_normally = false;
		bool b_copy_constructed = false;
		bool b_move_constructed = false;
	};

	/*
	* A Cpp17CopyConstructible, and Cpp17Destructible type
	*/
	class test_class_copy
	{
	public:
		explicit test_class_copy(int value) :
			val(value),
			b_constructed_normally(true)
		{}

		test_class_copy(const test_class_copy& other) noexcept :
			val(other.val),
			b_copy_constructed(true)
		{}
		test_class_copy(test_class_copy&& other) = delete;

		~test_class_copy() noexcept {}

	public:
		int val;
		// true iff constructed not by copy ctor
		bool b_constructed_normally = false;
		bool b_copy_constructed = false;
	};
}