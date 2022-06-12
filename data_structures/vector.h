#pragma once

#include <initializer_list>
#include <memory>

namespace ghl
{
	/*
	* ADT vector
	* underlying DS: array
	*
	* A vector (an array) of multiple objects of the same type T,
	* which must be : 
	* Cpp17CopyConstructible, 
	* and Cpp17Destructible
	*
	* Rep Invariant:
	* 1. size <= capacity
	* 2. start != nullptr (except for moved vectors)
	*
	* After moving a vector, using it results in undefined behavior.
	*
	* Thread-safety: No
	*
	* Design features:
	* 1. objects are allocated continuously on memory
	* 2. random access to objects is O(1), which is done by operator[]
	* 3. insertion or deletion at random location, however, may cause all or some of the objects to be copied,
	*	which are O(n)
	* 4. const (immutable) vector cannot change elements within it. So all functions that returns a non-const ptr or ref to a object is declared as non-const
	*
	*/
	template <typename T>
	class vector final
	{
		// used for unit test
		friend class vector_tester;
#pragma region constructors and destructors
	public:
		// the default construtor allocates 1 element of memory to ensure that start is never nullptr once constructed.
		vector() : vector(0) {}
		/*
		* Definted to support list-initialization
		* O(n)
		*/
		vector(const std::initializer_list<T>& in_list) :
			vector(in_list.size())
		{
			m_size = in_list.size();

			size_t i = 0;
			for (const auto& o : in_list)
			{
				new (mp_start + i) T(o);
				++i;
			}
		}
		
		/*
		* Requires random access iter
		*/
		template <typename Iter>
		vector(Iter begin, Iter end)
			: vector((end - begin > 0) ? (end - begin) : 0)
		{
			if (end - begin > 0)
			{
				m_size = end - begin;

				size_t ind = 0;
				for (auto iter = begin; iter != end; ++iter)
				{
					new (mp_start + ind) T(*iter);
					++ind;
				}
			}
		}

		/*
		* Allocates count number of objects on memory but do not initialize them
		* 
		* @param number of elements the user wants to allocate. If it is 0, it is treated as 1
		*/
		explicit vector(size_t count) : m_capacity((0 == count) ? 1 : count)
		{
			// need not to set size

			// allocate such amount of memory on heap. If count = 0, allocate 1 element space
			mp_start = (T*)malloc((0 == count) ? sizeof(T) : (sizeof(T) * count));

			if (nullptr == mp_start)
			{
				throw new std::bad_alloc();
			}
			
		}

		// O(n)
		vector(const vector& other) :
			vector(other.m_capacity)
		{
			m_size = other.m_size;
			for (size_t i = 0; i != m_capacity; ++i)
			{
				new (mp_start + i) T(*(other.mp_start + i));
			}
		}
		// O(1)
		vector(vector&& other) noexcept :
			m_capacity(other.m_capacity),
			m_size(other.m_size),
			mp_start(other.mp_start)
		{
			other.m_capacity = 0;
			other.m_size = 0;
			other.mp_start = nullptr;
		}

		~vector() noexcept
		{
			// we have to explicitly destroy elements since we use placement new
			if (0 != m_size && nullptr != mp_start)
			{
				for (size_t i = 0; i != m_size; ++i)
				{
					(mp_start + i)->~T();
				}

				// start is not nullptr
				free((void*)mp_start);
			}
		}
#pragma endregion

#pragma region assignments
		vector& operator=(const vector& right)
		{
			_ASSERT(check_rep());

			if (right.m_size > m_capacity) // need a larger buffer
			{
				m_capacity = right.m_size;

				// free the memory
				{
					// we have to explicitly destroy elements since we use placement new
					if (0 != m_size)
					{
						for (size_t i = 0; i != m_size; ++i)
						{
							(mp_start + i)->~T();
						}
					}
					// start is not nullptr
					free(mp_start);
				}

				// allocate the buffer and copy the elements
				{
					mp_start = (T*)malloc(sizeof(T) * right.m_size);
					for (size_t i = 0; i != right.m_size; ++i)
					{
						new (mp_start + i) T(right[i]);
					}
				}
			}
			else
			{
				// we have to explicitly destroy elements since we use placement new
				if (0 != m_size)
				{
					for (size_t i = 0; i != m_size; ++i)
					{
						(mp_start + i)->~T();
					}
				}
				// copy the elements
				for (size_t i = 0; i != right.m_size; ++i)
				{
					new (mp_start + i) T(right[i]);
				}
			}

			// assign capacity at the end cause we need it before
			m_size = right.m_size;

			return *this;
		}
		vector& operator=(vector&& right)
		{
			// free the memory
			{
				// we have to explicitly destroy elements since we use placement new
				if (0 != m_size)
				{
					for (size_t i = 0; i != m_size; ++i)
					{
						(mp_start + i)->~T();
					}
				}
				// start is not nullptr
				free(mp_start);
			}

			m_size = right.m_size;
			m_capacity = right.m_capacity;
			mp_start = right.mp_start;

			right.m_size = right.m_capacity = 0;
			right.mp_start = nullptr;

			return *this;
		}

#pragma endregion

#pragma region iterators
	public:
		using iterator = T*;
		using const_iterator = const T*;

		// supports ranged-for

		iterator begin() { return mp_start; }
		iterator end() { return mp_start + m_size; }
		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

		const_iterator cbegin() const { return mp_start; }
		const_iterator cend() const { return mp_start + m_size; }
#pragma endregion

#pragma region operations
	public:
		/*
		* Gets the i^th element. i must be in the valid index range. Otherwise, the behavior is undefined.
		* @returns a reference to the i^th element, if i is in range
		*/
		inline T& operator[](size_t i) { _ASSERT(i < m_size); return *(mp_start + i); }
		/*
		* Gets the i^th element from an immutable vector. i must be in the valid index range. Otherwise, the behavior is undefined.
		* @returns a const reference to the i^th element, if i is in range
		*/
		inline const T& operator[](size_t i) const { _ASSERT(i < m_size); return *(mp_start + i); }

		// @returns if the vector is empty
		inline bool empty() const { return 0 == m_size; }

		inline auto size() const { return m_size; }
		inline auto capacity() const { return m_capacity; }

		/*
		* Checks the representation invariant.
		* Never call it on moved vectors!
		* @return true iff the invariant holds
		*/
		inline bool check_rep() const { return m_size <= m_capacity; }

		/*
		* Resizes the vector so that it can contain at least new_size elements
		*/
		void resize(size_t new_size)
		{
			if (new_size > m_capacity) // need a larger buffer
			{
				auto new_start = (T*)malloc(sizeof(T) * new_size);

				if (nullptr != new_start)
				{
					// move previous objects to the new location
					for (size_t i = 0; i != m_size; ++i)
					{
						new (new_start + i) T(*(mp_start + i));
					}

					m_capacity = new_size;
				}
				else
				{
					throw new std::bad_alloc();
				}

				// free the previous memory
				{
					// we have to explicitly destroy elements since we use placement new
					if (0 != m_size)
					{
						for (size_t i = 0; i != m_size; ++i)
						{
							(mp_start + i)->~T();
						}
					}
					// start is not nullptr
					free(mp_start);
				}

				// assign start
				mp_start = new_start;
			}
		}
		/*
		* After a call to resize, which increased the capacity of the vector,
		* the method may be called to set the size to a higher value.
		* 
		* Only sets the size when prev size() <= new_size <= capacity()
		* 
		* By calling this, the user guaranteens that all objects between the previous size and the new size are properly constructed,
		* or at least will be constructed before the next call to the vector's methods.
		*/
		void increase_size(size_t new_size)
		{
			if (new_size <= m_capacity && m_size < new_size)
			{
				m_size = new_size;
			}
		}

		/*
		* Emplaces (constructs) an element at the end of the array (end of capacity) with arguments args...
		*
		* @param args... the arguments to be passed to the constructor of T
		* @return the iterator to the newly added element if succeeded; otherwise end();
		*/
		template <typename... P>
		iterator emplace_back(P&&... args)
		{
			_ASSERT(check_rep());

			++m_size;
			if (m_size > m_capacity) // need a larger buffer
			{
				// until the increment,
				// size indicates the previous capacity of objects
				auto new_start = (T*)malloc(sizeof(T) * m_size);

				if (nullptr != new_start)
				{
					// move previous objects to the new location
					for (size_t i = 0; i != m_capacity; ++i)
					{
						new (new_start + i) T(*(mp_start + i));
					}

					// free all previous elements
					if (nullptr != mp_start && 0 != m_capacity)
					{
						// we have to explicitly destroy them since we use placement new
						for (size_t i = 0; i != m_capacity; ++i)
						{
							(mp_start + i)->~T();
						}

						free(mp_start);
					}

					// emplace the new element
					new (new_start + m_capacity) T(std::forward<P>(args)...);

					// assign start
					mp_start = new_start;

					// size can only be one element smaller, and we make it equal to capacity
					++m_capacity;

					return new_start + m_size - 1;
				}
			}
			else
			{
				// emplace the new element
				new (mp_start + m_size - 1) T(std::forward<P>(args)...);

				return mp_start + m_size - 1;
			}

			return end();
		}

		/*
		* Copies or moves obj to the end of the array (end of capacity)
		*
		* @param obj the object to be copied
		* @return the iterator to the newly added element if succeeded; otherwise end();
		*/
		iterator push_back(const T& obj)
		{
			_ASSERT(check_rep());

			++m_size;
			if (m_size > m_capacity) // need a larger buffer
			{
				// until the increment of size,
				// size indicates the previous capacity of objects
				auto new_start = (T*)malloc(sizeof(T) * m_size);

				if (nullptr != new_start)
				{
					// move previous objects to the new location
					for (size_t i = 0; i != m_capacity; ++i)
					{
						new (new_start + i) T(*(mp_start + i));
					}

					// free all previous elements
					if (nullptr != mp_start && 0 != m_capacity)
					{
						// we have to explicitly destroy them since we use placement new
						for (size_t i = 0; i != m_capacity; ++i)
						{
							(mp_start + i)->~T();
						}

						free(mp_start);
					}

					// add the new element
					new (new_start + m_capacity) T(obj);

					// assign start
					mp_start = new_start;

					// size can only be one element smaller, and we make it equal to capacity
					++m_capacity;

					return new_start + m_size - 1;
				}
			}
			else
			{
				// emplace the new element
				new (mp_start + m_size - 1) T(obj);

				return mp_start + m_size - 1;
			}

			return end();
		}

		/*
		* Removes the element at the back of the vector, if there is one.
		* The operation does not shrink the capacity, it only decreases the size.
		*/
		void remove_back() noexcept
		{
			if (m_size != 0)
			{
				--m_size;
				(mp_start + m_size)->~T();
			}
		}

#pragma endregion

	private:
		// the size of the memory allocated, in sizeof(T)
		size_t m_capacity = 0;
		// the size of the objects constructed on the memory
		size_t m_size = 0;
		T* mp_start = nullptr;
	};
}