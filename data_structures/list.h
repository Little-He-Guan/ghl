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
	* 1. constant time insertion and deletion at random position
	* 2. const list is immutable, which does not allow modifications to its elements (if not counting iterators)
	* 3. As long as there are iterators pointing to a node, it is not destroyed (achieved by shared_ptr)
	* 
	* Thread-safety: No
	* 
	* Invariant: Either
	* 1. head = nullptr, or
	* 2. head = tail, or
	* 3. head reaches tail in finite steps
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

	public:
#pragma region iterator
		// a partially random access (not fully satisfies LegacyRandomAccessIterator) iter
		// which participates in owning the object it refers to
		struct const_iterator
		{
			const_iterator() : n(nullptr) {}
			const_iterator(const std::shared_ptr<node>& nn, bool is_end = false) : n(nn), b_end(is_end) {}

			const_iterator(const const_iterator& other) : n(other.n), b_end(other.b_end) {}
			const_iterator(const_iterator&& other) : n(std::move(other.n)), b_end(other.b_end) {}

			inline const_iterator& operator=(const const_iterator& right) noexcept { n = right.n; b_end = right.b_end; return *this; }
			inline const_iterator& operator=(const_iterator&& right) noexcept { n = std::move(right.n); b_end = right.b_end; return *this; }

			inline bool operator==(const const_iterator& right) const { return n == right.n && b_end == right.b_end; }
			inline bool operator!=(const const_iterator& right) const { return n != right.n || b_end != right.b_end; }

			// define it as non-const so that only non-const iterator can call it
			inline const T& operator*() const { return *(n->obj); }
			inline const T* operator->() const { return n->obj.get(); }

			bool valid() const { return nullptr != n; }

			// prefix
			const_iterator& operator++()
			{
				if (n->has_next())
				{
					n = n->next;
				}
				else // moving tail forward once results into end
				{
					b_end = true;
				}
				return *this;
			}
			// postfix
			const_iterator operator++(int)
			{
				auto temp = *this;
				if (n->has_next())
				{
					n = n->next;
				}
				else // moving tail forward once results into end
				{
					b_end = true;
				}
				return temp;
			}

			// prefix
			const_iterator& operator--()
			{
				if (b_end) // moving tail forward once results into end
				{
					b_end = false;
				}
				else
				{
					n = n->prev.lock();
				}
				return *this;
			}
			// postfix
			const_iterator operator--(int)
			{
				auto temp = *this;
				if (b_end) // moving tail forward once results into end
				{
					b_end = false;
				}
				else
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
			size_t operator-(const const_iterator& right) const
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
			const_iterator operator+(size_t offset) const
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
			const_iterator operator-(size_t offset) const
			{
				auto res = *this;
				for (size_t i = 0; i != offset; ++i)
				{
					--res;
				}
				return res;
			}

			// marks if the iterator is obtained by incrementing tail once
			// an end iter is the iter whose n is tail and whose b_end is true
			bool b_end = false;
			std::shared_ptr<node> n;
		};

		// a partially random access (not fully satisfies LegacyRandomAccessIterator) iter
		// which participates in owning the object it refers to
		struct iterator
		{
			iterator() : n(nullptr) {}
			iterator(const std::shared_ptr<node>& nn, bool is_end = false) : n(nn), b_end(is_end) {}

			iterator(const iterator& other) : n(other.n), b_end(other.b_end) {}
			iterator(iterator&& other) : n(std::move(other.n)), b_end(other.b_end) {}

			inline iterator& operator=(const iterator& right) noexcept { n = right.n; b_end = right.b_end; return *this; }
			inline iterator& operator=(iterator&& right) noexcept { n = std::move(right.n); b_end = right.b_end; return *this; }

			inline bool operator==(const iterator& right) const { return n == right.n && b_end == right.b_end; }
			inline bool operator!=(const iterator& right) const { return n != right.n || b_end != right.b_end; }

			// define it as non-const so that only non-const iterator can call it
			inline T& operator*() const { return *(n->obj); }
			inline T* operator->() const { return n->obj.get(); }

			bool valid() const { return nullptr != n; }

			inline operator const_iterator() { return const_iterator(n, b_end); }

			// prefix
			iterator& operator++() 
			{ 
				if (n->has_next())
				{
					n = n->next;
				}
				else // moving tail forward once results into end
				{
					b_end = true;
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
				else // moving tail forward once results into end
				{
					b_end = true;
				}
				return temp;
			}

			// prefix
			iterator& operator--()
			{
				if (b_end) // moving tail forward once results into end
				{
					b_end = false;
				}
				else
				{
					n = n->prev.lock();
				}
				return *this;
			}
			// postfix
			iterator operator--(int)
			{
				auto temp = *this;
				if (b_end) // moving tail forward once results into end
				{
					b_end = false;
				}
				else
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
			iterator operator+(size_t offset) const
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
			iterator operator-(size_t offset) const
			{
				auto res = *this;
				for (size_t i = 0; i != offset; ++i)
				{
					--res;
				}
				return res;
			}

			// marks if the iterator is obtained by incrementing tail once
			// an end iter is the iter whose n is tail and whose b_end is true
			bool b_end = false;
			std::shared_ptr<node> n;
		};

		iterator begin() { return empty() ? end() : iterator(head); }
		iterator end() { return iterator(tail.lock(), true); }
		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

		const_iterator cbegin() const { return empty() ? cend() : const_iterator(iterator(head)); }
		const_iterator cend() const { return const_iterator(tail.lock(), true); }
#pragma endregion

	public:
#pragma region operations
		/*
		* Checks the invariant
		* 
		* Warning: High runtime cost, should only be used in tests
		*/
		bool check_rep() const
		{
			if (nullptr == head)
			{
				return true;
			}
			else if (head == tail.lock())
			{
				return true;
			}
			else
			{
				std::weak_ptr<node> curr = head;
				while (curr.lock() != tail.lock())
				{
					curr = curr.lock()->next;
					if (curr.expired())
					{
						return false;
					}
				}
				return true;
			}

			return false;
		}

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
			// if inserting before end, tail has to be updated
			// if inserting before head, head has to be updated

			// before
			// prev <-> (pos.n) <-> next
			// if pos is begin, then it's like
			// nullptr <- (pos.n) <-> next
			// if pos is end, then it's like
			// prev <-> (pos.n) -> nullptr

			if (!empty()) // not empty
			{
				if (pos.b_end) // pos is end
				{
					auto new_node = std::make_shared<node>(new T(ele), tail, nullptr);

					tail.lock()->next = new_node;
					tail = new_node;

					return iterator(tail.lock());
				}
				else if (pos.n == head) // pos is head
				{
					auto new_node = std::make_shared<node>(new T(ele), std::shared_ptr<node>(), head);

					head->prev = new_node;
					head = new_node;

					return iterator(head);
				}
				else
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
			}
			else // if it is empty, then we can assume that pos = begin or pos = end. Either way, we make head and tail the element.
			{
				// make a new node that connects pos.n->prev and pos.n
				tail = (head = std::make_shared<node>(new T(ele)));

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
			// if inserting before end, tail has to be updated
			// if inserting before head, head has to be updated

			// before
			// prev <-> (pos.n) <-> next
			// if pos is begin, then it's like
			// nullptr <- (pos.n) <-> next
			// if pos is end, then it's like
			// prev <-> (pos.n) -> nullptr

			if (!empty()) // not empty
			{
				if (pos.b_end) // pos is end
				{
					auto new_node = std::make_shared<node>(new T(std::forward<P>(args)...), tail, nullptr);

					tail.lock()->next = new_node;
					tail = new_node;

					return iterator(tail.lock());
				}
				else if (pos.n == head) // pos is head
				{
					auto new_node = std::make_shared<node>(new T(std::forward<P>(args)...), std::shared_ptr<node>(), head);

					head->prev = new_node;
					head = new_node;

					return iterator(head);
				}
				else
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
			}
			else // if it is empty, then we can assume that pos = begin or pos = end. Either way, we make head and tail the element.
			{
				// make a new node that connects pos.n->prev and pos.n
				tail = (head = std::make_shared<node>(new T(std::forward<P>(args)...)));

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
		* If the list is not empty, then pos must be in [begin, end). Otherwise, the behaviour is undefined.
		* 
		* @returns iterator (could be end) to the next element of pos (before removing), or end if empty or pos is end.
		*/
		iterator remove(iterator pos)
		{
			// if removing head or tail, it needs to be updated

			if (!empty() && pos.b_end != true)
			{
				bool b_is_tail;
				if (b_is_tail = !(pos.n->has_next())) // pos.n == tail
				{
					tail = tail.lock()->prev;
				}
				else // pos.n != tail
				{
					// unlink pos.n->next->prev and relink it to pos.n->prev
					pos.n->next->prev = pos.n->prev;
				}

				bool b_is_head;
				if (b_is_head = !(pos.n->has_prev()))
				{
					if (b_is_tail) // if both head and tail, after removing the list will be empty
					{
						tail = (head = nullptr);
					}
					else
					{
						head = pos.n->next;
					}
				}
				else
				{
					// unlink pos.n->prev->next and relink it to pos.n->next
					(pos.n->prev).lock()->next = pos.n->next;
				}

				if (b_is_tail)
				{
					return end();
				}
				if(b_is_head)
				{
					return iterator(head);
				}
				else
				{
					return (pos.n->prev).lock()->next;
				}
			}

			return end();
		}

		/*
		* Remove the element at begin
		*/
		inline iterator remove_front()
		{
			return remove(begin());
		}

		/*
		* Remove the element before end
		*/
		inline iterator remove_back()
		{
			return remove(iterator(tail.lock()));
		}
#pragma endregion

	private:
		std::shared_ptr<node> head = nullptr;
		std::weak_ptr<node> tail;
	};
}