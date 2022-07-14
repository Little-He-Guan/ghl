#pragma once

#include "vector.h"

namespace ghl
{
	/*
	* Base class for all binary heaps
	* (The index exposed to users starts from 1, not 0)
	*/
	template <typename T>
	class binary_heap
	{
	public:
		// gives an empty heap
		binary_heap() {}
		//// inits the heap with an unordered list of elements, O(n)
		//binary_heap(const std::initializer_list<T>& in_list)
		//	: data(in_list), heap_size(in_list.size())
		//{
		//	for (size_t i = data.size() / 2; i != 0; --i)
		//	{
		//		max_heapify(i);
		//	}
		//}

		virtual ~binary_heap() {}

	public:
		static constexpr inline size_t left(size_t ind) { return 2 * ind; }
		static constexpr inline size_t right(size_t ind) { return 2 * ind + 1; }
		static constexpr inline size_t parent(size_t ind) { return ind / 2; /* floor(ind/2) */ }

	public:
		// Note: the behaviour is undefined if the index is out of bound
		// @returns reference to the element at index i, but does not remove it.
		virtual const T& operator[](size_t i) const = 0;
		virtual T& operator[](size_t i) = 0;

		// Note: the behaviour is undefined if the index is out of bound
		// @returns a reference to the top element (max or min depending on the derived class) but does not remove it from the heap 
		T& top() { return this->operator[](1); }
		const T& top() const { return this->operator[](1); }

	public:
		// Note: the behaviour is undefined if the heap is empty
		// @returns a copy to the top element and removes the it from the heap
		virtual T extract_top() = 0;

		/*
		* Insert a new element into the heap
		*/
		virtual void insert(T val) = 0;

		/*
		* Sets the value at index i to new_val, while maintaining the property
		* The final value at i after the call consequently may not be new_val.
		* 
		* Note: derived classes may strengthen the specification in limiting new_val to be bigger/smaller than the original value.
		*/
		virtual void update_element(size_t i, T new_val) = 0;

	protected:
		/*
		* heapify the heap starting from index i
		* It assumes that the subtrees at left(i) and right(i) satisfy the property, but the subtree at i might not.
		* It does nothing if i is out of bound.
		*/
		virtual void heapify(size_t i) = 0;

	public:
		virtual size_t size() const = 0;
		bool empty() const { return 0 == size(); }

	protected:
	};

	// implementation of max-heap for light-weight objects (that will be stored in-place)
	template <typename T>
	class max_heap_lw final : public binary_heap<T>
	{
		// to test private methods, we need an intrusive design
		friend class max_heap_lw_tester;

	private:
		using super = binary_heap<T>;

	public:
		max_heap_lw() : super() {}
		explicit max_heap_lw(const std::initializer_list<T>& init_list) : 
			max_heap_lw(), data(init_list)
		{
			for (size_t i = data.size() / 2; i >= 1; --i)
			{
				heapify(i);
			}
		}
		~max_heap_lw() {}

	public:
		const T& operator[](size_t i) const override { return data[i-1]; }
		T& operator[](size_t i) override { return data[i-1]; }

		size_t size() const override { return data.size(); }

	public:
		T extract_top() override
		{
			T res = data[0];

			if (!this->empty())
			{
				// move the last element to the top
				data[0] = data[data.size() - 1];
				data.remove_back();

				heapify(1);
			}

			return res;
		}
		
		void insert(T val) override
		{
			data.push_back(val);

			// maintain the heap property
			size_t i = data.size();
			while (i > 1 && data[super::parent(i) - 1] < data[i - 1])
			{
				std::swap(data[super::parent(i) - 1], data[i - 1]);
				i = super::parent(i);
			}
		}

		/*
		* Here we limit new_val to be bigger than the value at i
		* If the arguments supplied are otherwise, the behaviour is undefined.
		*/
		void update_element(size_t i, T new_val) override
		{
			if (new_val > data[i - 1])
			{
				data[i - 1] = new_val;

				// maintain the property
				while (i > 1 && data[super::parent(i) - 1] < data[i - 1])
				{
					std::swap(data[super::parent(i) - 1], data[i - 1]);
					i = super::parent(i);
				}
			}
		}
		
	protected:
		void heapify(size_t i) override
		{			
			// max-heapify

			size_t heap_size = size();

			if (i > 0 && i < heap_size)
			{
				size_t l = super::left(i), r = super::right(i);
				size_t largest_ind = i;

				if (l <= heap_size && data[l - 1] > data[i - 1]) // according to the execution order of a && operator, only if l <= size will data[l-1] be accessed
				{
					largest_ind = l;
				}
				if (r <= heap_size && data[r - 1] > data[largest_ind - 1]) // similar to the previous comment
				{
					largest_ind = r;
				}

				if (largest_ind != i)
				{
					std::swap(data[i - 1], data[largest_ind - 1]);
					heapify(largest_ind);
				}
			}
		}

	private:
		ghl::vector<T> data;
	};

	/*
	* implementation of max-heap for heavy-weight objects(that will be stored via pointers)
	* Note: 
	* 1. the heap does not have the ownership of the objects.
	* 2. if two or more pointers to the same place are added in the heap by any means, the behaviour is undefined
	*/ 
	template <typename T>
	class max_heap_hw final : public binary_heap<T*> // we allocate hw objects dynamically and refer to them by pointers
	{
	private:
		using super = binary_heap<T*>;
	public:
		using obj_t = T;
		using ptr_t = T*;

	public:
		max_heap_hw() : super() {}
		explicit max_heap_hw(const std::initializer_list<ptr_t>& init_list) :
			max_heap_hw(), data(init_list)
		{
			for (size_t i = data.size() / 2; i >= 1; --i)
			{
				heapify(i);
			}
		}
		~max_heap_hw() {}

	public:
		const ptr_t& operator[](size_t i) const override { return data[i - 1]; }
		ptr_t& operator[](size_t i) override { return data[i - 1]; }

		size_t size() const override { return data.size(); }

	public:
		ptr_t extract_top() override
		{
			ptr_t res = data[0];

			if (!this->empty())
			{
				// move the last element to the top
				data[0] = data[data.size() - 1];
				data.remove_back();

				heapify(1);
			}

			return res;
		}

		void insert(ptr_t val) override
		{
			data.push_back(val);

			// maintain the heap property
			size_t i = data.size();
			while (i > 1 && *data[super::parent(i) - 1] < *data[i - 1])
			{
				std::swap(data[super::parent(i) - 1], data[i - 1]);
				i = super::parent(i);
			}
		}

		/*
		* Here we limit *new_val to be bigger than the value pointed to by the ptr at i
		* If the arguments supplied are otherwise, the behaviour is undefined.
		*/
		void update_element(size_t i, ptr_t new_val) override
		{
			if (*new_val > *data[i - 1])
			{
				data[i - 1] = new_val;

				// maintain the property
				while (i > 1 && *data[super::parent(i) - 1] < *data[i - 1])
				{
					std::swap(data[super::parent(i) - 1], data[i - 1]);
					i = super::parent(i);
				}
			}
		}

	protected:
		void heapify(size_t i) override
		{
			// max-heapify

			size_t heap_size = size();

			if (i > 0 && i < heap_size)
			{
				size_t l = super::left(i), r = super::right(i);
				size_t largest_ind = i;

				if (l <= heap_size && *data[l - 1] > *data[i - 1]) // according to the execution order of a && operator, only if l <= size will data[l-1] be accessed
				{
					largest_ind = l;
				}
				if (r <= heap_size && *data[r - 1] > *data[largest_ind - 1]) // similar to the previous comment
				{
					largest_ind = r;
				}

				if (largest_ind != i)
				{
					std::swap(data[i - 1], data[largest_ind - 1]);
					heapify(largest_ind);
				}
			}
		}

	private:
		ghl::vector<ptr_t> data;
	};

	// implementation of max-heap for light-weight objects (that will be stored in-place)
	template <typename T>
	class min_heap_lw final : public binary_heap<T>
	{
	private:
		using super = binary_heap<T>;

	public:
		min_heap_lw() : super() {}
		explicit min_heap_lw(const std::initializer_list<T>& init_list) :
			min_heap_lw(), data(init_list)
		{
			for (size_t i = data.size() / 2; i >= 1; --i)
			{
				heapify(i);
			}
		}
		~min_heap_lw() {}

	public:
		const T& operator[](size_t i) const override { return data[i - 1]; }
		T& operator[](size_t i) override { return data[i - 1]; }

		size_t size() const override { return data.size(); }

	public:
		T extract_top() override
		{
			T res = data[0];

			if (!this->empty())
			{
				// move the last element to the top
				data[0] = data[data.size() - 1];
				data.remove_back();

				heapify(1);
			}

			return res;
		}

		void insert(T val) override
		{
			data.push_back(val);

			// maintain the heap property
			size_t i = data.size();
			while (i > 1 && data[super::parent(i) - 1] > data[i - 1])
			{
				std::swap(data[super::parent(i) - 1], data[i - 1]);
				i = super::parent(i);
			}
		}

		/*
		* Here we limit new_val to be smaller than the value at i
		* If the arguments supplied are otherwise, the behaviour is undefined.
		*/
		void update_element(size_t i, T new_val) override
		{
			if (new_val < data[i - 1])
			{
				data[i - 1] = new_val;

				// maintain the property
				while (i > 1 && data[super::parent(i) - 1] > data[i - 1])
				{
					std::swap(data[super::parent(i) - 1], data[i - 1]);
					i = super::parent(i);
				}
			}
		}

	protected:
		void heapify(size_t i) override
		{
			// max-heapify

			size_t heap_size = size();

			if (i > 0 && i < heap_size)
			{
				size_t l = super::left(i), r = super::right(i);
				size_t smallest_ind = i;

				if (l <= heap_size && data[l - 1] < data[i - 1]) // according to the execution order of a && operator, only if l <= size will data[l-1] be accessed
				{
					smallest_ind = l;
				}
				if (r <= heap_size && data[r - 1] < data[smallest_ind - 1]) // similar to the previous comment
				{
					smallest_ind = r;
				}

				if (smallest_ind != i)
				{
					std::swap(data[i - 1], data[smallest_ind - 1]);
					heapify(smallest_ind);
				}
			}
		}

	private:
		ghl::vector<T> data;
	};

	/*
	* implementation of max-heap for heavy-weight objects(that will be stored via pointers)
	* Note:
	* 1. the heap does not have the ownership of the objects.
	* 2. if two or more pointers to the same place are added in the heap by any means, the behaviour is undefined
	*/
	template <typename T>
	class min_heap_hw final : public binary_heap<T*> // we allocate hw objects dynamically and refer to them by pointers
	{
		// to test private methods, we need an intrusive design
		friend class min_heap_hw_tester;
	private:
		using super = binary_heap<T*>;
	public:
		using obj_t = T;
		using ptr_t = T*;

	public:
		min_heap_hw() : super() {}
		explicit min_heap_hw(const std::initializer_list<ptr_t>& init_list) :
			min_heap_hw(), data(init_list)
		{
			for (size_t i = data.size() / 2; i >= 1; --i)
			{
				heapify(i);
			}
		}
		~min_heap_hw() {}

	public:
		const ptr_t& operator[](size_t i) const override { return data[i - 1]; }
		ptr_t& operator[](size_t i) override { return data[i - 1]; }

		size_t size() const override { return data.size(); }

	public:
		ptr_t extract_top() override
		{
			ptr_t res = data[0];

			if (!this->empty())
			{
				// move the last element to the top
				data[0] = data[data.size() - 1];
				data.remove_back();

				heapify(1);
			}

			return res;
		}

		void insert(ptr_t val) override
		{
			data.push_back(val);

			// maintain the heap property
			size_t i = data.size();
			while (i > 1 && *data[super::parent(i) - 1] > *data[i - 1])
			{
				std::swap(data[super::parent(i) - 1], data[i - 1]);
				i = super::parent(i);
			}
		}

		/*
		* Here we limit *new_val to be smaller than the value pointed to by the ptr at i
		* If the arguments supplied are otherwise, the behaviour is undefined.
		*/
		void update_element(size_t i, ptr_t new_val) override
		{
			if (*new_val < *data[i - 1])
			{
				data[i - 1] = new_val;

				// maintain the property
				while (i > 1 && *data[super::parent(i) - 1] > *data[i - 1])
				{
					std::swap(data[super::parent(i) - 1], data[i - 1]);
					i = super::parent(i);
				}
			}
		}

	protected:
		void heapify(size_t i) override
		{
			// max-heapify

			size_t heap_size = size();

			if (i > 0 && i < heap_size)
			{
				size_t l = super::left(i), r = super::right(i);
				size_t smallest_ind = i;

				if (l <= heap_size && *data[l - 1] < *data[i - 1]) // according to the execution order of a && operator, only if l <= size will data[l-1] be accessed
				{
					smallest_ind = l;
				}
				if (r <= heap_size && *data[r - 1] < *data[smallest_ind - 1]) // similar to the previous comment
				{
					smallest_ind = r;
				}

				if (smallest_ind != i)
				{
					std::swap(data[i - 1], data[smallest_ind - 1]);
					heapify(smallest_ind);
				}
			}
		}

	private:
		ghl::vector<ptr_t> data;
	};
}