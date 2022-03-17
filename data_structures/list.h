#pragma once

#include <initializer_list>
#include <memory>

namespace ghl
{
	/*
	* A two direction linked list of objects of type T,
	* which must satisfy the requirments of std::unique_ptr
	* 
	* Features:
	* 1. object will not be destroyed even if it's not in the list anymore, as long as there are some iterators pointing to it. (achieved by shared_ptr)
	*	One side effect of the feature is that other elements that are not directly pointed to by the iter may be indirectly owned by the iter.
	* 2. constant time insertion and deletion at random position
	* 3. const list is immutable, which does not allow modifications to its elements
	* 
	* Thread-safety: No
	* 
	* Invariant: None
	*/
	template <typename T>
	class list final
	{
		// for unit tests
		friend class list_tester;
	private:
		struct node
		{
			/*
			* o is created by list, and is then acquired and managed by the node
			*/
			explicit node(T* o = nullptr, std::weak_ptr<node> bef = std::shared_ptr<node>(), const std::shared_ptr<node>& aft = nullptr) :
				obj(o), prev(bef), next(aft) {}
			~node() {}

			inline bool has_prev() const { return 0 != prev.use_count(); }
			inline bool has_next() const { return nullptr != next; }

			auto& get_obj() { return *obj; }
			const auto& get_obj() const { return *obj; }

			std::unique_ptr<T> obj;
			// to avoid circular ownership, prev does not own the node
			std::weak_ptr<node> prev;
			// to avoid circular ownership, only next owns the node
			std::shared_ptr<node> next;
		};

	public:
#pragma region constructors and destructor
		list() {}

		// supports list-initialization
		list(const std::initializer_list<T>& list)
		{
			if (list.size() != 0)
			{
				head = std::make_shared<node>(new T(*list.begin()));
				std::shared_ptr<node> curr = head;
				std::shared_ptr<node> next = nullptr;

				for (auto i = list.begin() + 1; i != list.end(); ++i)
				{
					next = std::make_shared<node>(new T(*i), curr);
					curr->next = next;
					curr = next;
				}

				tail = curr;
			}
		}

		/*
		* Iter must be a forward iter
		* begin should reach end after finite increments. Otherwise, the behaviour is undefined.
		*/
		template <typename Iter>
		list(Iter begin, Iter end)
		{
			if (begin != end)
			{
				head = std::make_shared<node>(new T(*begin));
				std::shared_ptr<node> curr = head;
				std::shared_ptr<node> next = nullptr;
				
				++begin;

				for (; begin != end; ++begin)
				{
					next = std::make_shared<node>(new T(*begin), curr);
					curr->next = next;
					curr = next;
				}

				tail = curr;
			}
		}

		// needs to do nothing as all resources are managed by smart pointers
		~list() {}
#pragma endregion

#pragma region iterator
		// a partially random access (not fully satisfies LegacyRandomAccessIterator) iter
		// which participates in owning the object it refers to
		struct iterator
		{
			// does not allow default iterator
			iterator() = delete;
			iterator(const std::shared_ptr<node>& nn) : n(nn) {};

			iterator(const iterator& other) : n(other.n) {}
			iterator(iterator&& other) : n(std::move(other.n)) {}

			inline iterator& operator=(const iterator& right) noexcept { n = right.n; return *this; }
			inline iterator& operator=(iterator&& right) noexcept { n = std::move(right.n); return *this; }

			inline bool operator==(const iterator& right) const { return n == right.n; }
			inline bool operator!=(const iterator& right) const { return n != right.n; }

			// define it as non-const so that only non-const iterator can call it
			inline T& operator*() { return *(n->obj); }
			inline const T& operator*() const { return *(n->obj); }
			inline T* operator->() { return n->obj.get(); }
			inline const T* operator->() const { return n->obj.get(); }

			// prefix
			iterator& operator++() 
			{ 
				if (n->has_next())
				{
					n = n->next;
				}
				return *this; 
			}
			// postfix
			iterator operator++(int) 
			{ 
				auto temp = *this; 
				if (n->has_next())
				{
					n = n->next;
				}
				return temp;
			}

			// prefix
			iterator& operator--()
			{
				if (n->has_prev())
				{
					n = n->prev.lock();
				}
				return *this;
			}
			// postfix
			iterator operator--(int)
			{
				auto temp = *this;
				if (n->has_prev())
				{
					n = n->prev.lock();
				}
				return temp;
			}

			/*
			* Calculates the distance between this and right,
			* which must all be in [begin, end], and right must be before or equal to this.
			* Otherwise, the behaviour is undefined.
			*/
			size_t operator-(const iterator& right) const
			{
				size_t dist = 0;
				/*
				* Why we use -- not ++ here 
				* Well that's because (if right = end) end()->prev = tail but tail->next = nullptr, and we can only go backwards by having
				*/
				for (auto i = *this; i != right; --i)
				{
					++dist;
				}
				return dist;
			}

			/*
			* Random access an element at an offset. O(n)
			* The user should guarantee that the result is with in [begin,end).
			* Otherwise, the behaviour is undefined.
			*/
			iterator operator+(size_t offset)
			{
				auto res = *this;
				for (size_t i = 0; i != offset; ++i)
				{
					++res;
				}
				return res;
			}

			/*
			* Random access an element at an inversed offset. O(n)
			* The user should guarantee that the result is with in [begin,end).
			* Otherwise, the behaviour is undefined.
			*/
			iterator operator-(size_t offset)
			{
				auto res = *this;
				for (size_t i = 0; i != offset; ++i)
				{
					--res;
				}
				return res;
			}

			std::shared_ptr<node> n;
		};

		// const_iterator and iterator only differs in operator*, which we overloaded to support this.
		using const_iterator = const iterator;

		iterator begin() { return iterator(head); }
		iterator end() { return iterator(std::make_shared<node>(nullptr, tail)); /* make a new node whose prev is tail */ }

		const_iterator cbegin() const { return (const_iterator)iterator(head); }
		const_iterator cend() const { return (const_iterator)iterator(std::make_shared<node>(nullptr, tail)); /* make a new node whose prev is tail */ }
#pragma endregion

	public:
#pragma region operations
		inline bool empty() const { return nullptr == head; }

		T& front() { return *(head->obj); }
		const T& front() const { return *(head->obj); }

		T& back() { return *(tail->obj); }
		const T& back() const { return *(tail->obj); }

		size_t size() const
		{
			if (empty())
			{
				return 0;
			}
			else
			{
				return cend() - cbegin();
			}
		}

		/*
		* Insert the element before position.
		* position must be in [begin, end]. Otherwise, the behaviour is undefined.
		* 
		* @param pos position
		* @param ele element
		* 
		* @return the iterator to the newly inserted element
		*/
		iterator insert(iterator pos, const T& ele)
		{
			// before
			// prev <-> (pos.n) <-> next
			// if pos is begin, then it's like
			// nullptr <- (pos.n) <-> next
			// if pos is end, then it's like
			// prev <-> (pos.n) -> nullptr

			if (!empty()) // not empty
			{
				// make a new node that connects pos.n->prev and pos.n
				auto new_node = std::make_shared<node>(new T(ele), pos.n->prev, pos.n);

				/* now   new
				*     /		 \
				*   prev <-> (pos.n) <-> next
				*/

				// unlink pos.n->prev and relink it to the new node
				// becaues new_node owns pos.n, it won't be released during the unlinking
				pos.n->prev = new_node;
				// unlink pos.n->prev->next (now new_node->prev->next) if it has a prev, and relink it to the new node
				if (new_node->has_prev())
				{
					new_node->prev.lock()->next = new_node;
				}

				/* now  new
				*     /|   |\
				*   prev   (pos.n) <-> next
				*/

				return iterator(new_node);
			}
			else // if it is empty, then we can assume that pos = begin or pos = end. Either way, we make head and tail the element.
			{
				// make a new node that connects pos.n->prev and pos.n
				tail = head = std::make_shared<node>(new T(ele));

				return iterator(head);
			}
		}

		/*
		* Insert the element before end
		*/
		inline iterator insert_back(const T& ele)
		{
			return insert(end(), ele);
		}

		/*
		* Insert the element before begin
		*/
		inline iterator insert_front(const T& ele)
		{
			return insert(begin(), ele);
		}

		template <typename... P>
		iterator emplace(iterator pos, P&&... args)
		{
			// before
			// prev <-> (pos.n) <-> next
			// if pos is begin, then it's like
			// nullptr <- (pos.n) <-> next
			// if pos is end, then it's like
			// prev <-> (pos.n) -> nullptr

			if (!empty()) // not empty
			{
				// make a new node that connects pos.n->prev and pos.n
				auto new_node = std::make_shared<node>(new T(std::forward<P>(args)...), pos.n->prev, pos.n);

				/* now   new
				*     /		 \
				*   prev <-> (pos.n) <-> next
				*/

				// unlink pos.n->prev and relink it to the new node
				// becaues new_node owns pos.n, it won't be released during the unlinking
				pos.n->prev = new_node;
				// unlink pos.n->prev->next (now new_node->prev->next) if it has a prev, and relink it to the new node
				if (new_node->has_prev())
				{
					new_node->prev.lock()->next = new_node;
				}

				/* now  new
				*     /|   |\
				*   prev   (pos.n) <-> next
				*/

				return iterator(new_node);
			}
			else // if it is empty, then we can assume that pos = begin or pos = end. Either way, we make head and tail the element.
			{
				// make a new node that connects pos.n->prev and pos.n
				tail = head = std::make_shared<node>(new T(std::forward<P>(args)...));

				return iterator(head);
			}
		}

		/*
		* Insert the element before end
		*/
		template <typename... P>
		inline iterator emplace_back(P&& ... args)
		{
			return emplace(end(), std::forward<P>(args)...);
		}

		/*
		* Insert the element before begin
		*/
		template <typename... P>
		inline iterator emplace_front(P&& ... args)
		{
			return emplace(begin(), std::forward<P>(args)...);
		}

		/*
		* Remove the element at pos
		* position must be in [begin, end). Otherwise, the behaviour is undefined.
		*/
		void remove(iterator pos)
		{
			if (!empty())
			{
				auto shared_tail = tail.lock();
				if (shared_tail == pos.n) // we are removing tail
				{
					tail = shared_tail->prev;
				}

				if (pos.n->has_prev())
				{
					// unlink pos.n->prev->next and relink it to pos.n->next
					pos.n->prev.lock()->next = pos.n->next;
				}

				if (pos.n->has_next())
				{
					// unlink pos.n->next->prev and relink it to pos.n->prev
					pos.n->next->prev = pos.n->prev;
				}

				// After this, all references to pos.n are gone, and pos.n will be destroyed by shared_ptr when all iterators  are destroyed.
			}
		}

		/*
		* Insert the element at begin
		*/
		inline void remove_front()
		{
			remove(begin());
		}

		/*
		* Insert the element before end
		*/
		inline void remove_back()
		{
			remove(iterator(tail.lock()));
		}
#pragma endregion

	private:
		// to avoid circular ownership, only head owns the node (and thus owns the list)
		// However, the ownership maybe shared by iteraters, which has a side effect.
		// See the comment of this class.
		std::shared_ptr<node> head = nullptr;
		std::weak_ptr<node> tail;
	};
}