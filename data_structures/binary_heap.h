#pragma once

#include "vector.h"

namespace ghl
{
	/*
	* A max-heap
	* 
	* if lightweight is true, T occupies a small amount of memory and can be easily copied, and it's stored in-place
	* otherwise, the objects are stored via pointers
	* 
	* Its indices start from 1, not 0.
	*/
	template <typename T, bool LightWeight = true>
	class binary_heap
	{
	public:
		// gives an empty heap
		binary_heap() {}
		// inits the heap with an unordered list of elements, O(n)
		binary_heap(const std::initializer_list<T>& in_list)
			: data(in_list), heap_size(in_list.size())
		{
			for (size_t i = data.size() / 2; i != 0; --i)
			{
				max_heapify(i);
			}
		}
	public:

	public:
		static constexpr inline size_t left(size_t ind) { return 2 * ind; }
		static constexpr inline size_t right(size_t ind) { return 2 * ind + 1; }
		static constexpr inline size_t parent(size_t ind) { return ind / 2; /* floor(ind/2) */ }

		T& operator[](size_t i) const { return data[i - 1]; }

	public:
		/*
		* If it is called with the subtrees at left(i) and right(i)
		*/
		void max_heapify(size_t i)
		{
			auto l = left(i), r = right(i);

			size_t largest_ind = i;
			if (l <= heap_size && data[l - 1] > data[largest_ind - 1])
			{
				largest_ind = l;
			}
			if (r <= heap_size && data[r - 1] > data[largest_ind - 1])
			{
				largest_ind = r;
			}

			if (largest_ind != i)
			{
				std::swap(data[i - 1], data[largest_ind - 1]);
				max_heapify(largest_ind);
			}
		}

	public:
		/*
		* returns the max
		*/
		T& max() const { return data[0]; }

		/*
		* returns a copy to the max and removes the max
		*/
		T extract_max()
		{
			T res = data[0];

			// move the last element to the top
			data[0] = data[heap_size - 1];
			--heap_size;

			max_heapify(1);

			return res;
		}
		
		/*
		* if bigger_val > the element at index i,
		* it is replaced with bigger_val.
		* 
		* The final value at the place after the call, however, may not be bigger_val, 
		* because the max heap property has to be maintained.
		*/
		void increase_element(size_t i, T bigger_val)
		{
			if (bigger_val > data[i - 1])
			{
				data[i - 1] = bigger_val;

				// maintain the property
				while (i > 1 && data[parent(i) - 1] < data[i - 1])
				{
					std::swap(data[parent(i) - 1], data[i - 1]);
					i = parent(i);
				}
			}
		}

		void insert(T val)
		{
			++heap_size;

			data.resize(heap_size);
			data.increase_size(heap_size);

			// construct an element = val at new heap_size - 1
			new (&data[heap_size - 2] + 1) T(val);

			// maintain the property
			size_t i = heap_size;
			while (i > 1 && data[parent(i) - 1] < data[i - 1])
			{
				std::swap(data[parent(i) - 1], data[i - 1]);
				i = parent(i);
			}
		}

	private:
		ghl::vector<T> data;
		// different from data.size
		size_t heap_size = 0;
	};
}