#pragma once

#include "../data_structures/vector.h" // used for storing copied elements in merge_sort

namespace ghl
{
	/*
	* The list is divided into left subseq and right subseq
	* The minimal element of the right seq BUBBLEs to the left seq
	*
	* Invariant:
	* 1. all elements in the left subsequence are lower bounds of the right subseq
	* 2. left subseq is sorted in ascending order
	* 
	* Requirements:
	* 1. T is a LegacyRandomAccessIterator and a LegacyOutputIterator
	* 2. the type of the object must have an operator<
	* 3. the type of the object must be Cpp17CopyConstructible, Cpp17CopyAssignable
	*/
	template <typename T>
	void bubble_sort(T begin, T end)
	{
		// left subseq: [begin, leftBound) (the empty set if leftBound = begin)
		// right subseq: [leftBound, end)

		if (end - begin >= 2)
		{
			T leftBound = begin;
			for (; leftBound != end; ++leftBound) // each iteration forwards leftBound by 1, and increases left subseq by 1
			{
				// the second last element (always valid as begin - end >= 2)
				auto rightMinIndex = end - 2;
				/*
				* The invariant for the inner loop: *(rightMinIndex) is the min of all elements to the right of the index
				* but it will be established after the first iteration
				*/
				for (; rightMinIndex >= leftBound; --rightMinIndex)
				{
					if (*(rightMinIndex + 1) < *rightMinIndex) // the one to the right of the min is smaller than the min
					{
						// swap the two
						auto temp = *(rightMinIndex + 1); // Cpp17CopyConstructible
						*(rightMinIndex + 1) = *rightMinIndex; // Cpp17CopyAssignable
						*rightMinIndex = temp; // Cpp17CopyAssignable
					}
				}
			}
		}
	}

	/*
	* Overloading to bubble_sort
	* Equivalent to bubble_sort(list.begin(), list.end());
	*/
	template <typename T>
	void bubble_sort(T & list)
	{
		bubble_sort(list.begin(), list.end());
	}

	/*
	* Sorting by inserting elements from right subseq to left subseq (insertion here causes swapping some elements in a range)
	*
	* Invariant:
	* left seq is sorted in ascending order
	* 
	* Requirements:
	* 1. T is a LegacyRandomAccessIterator and a LegacyOutputIterator
	* 2. the type of the object must have an operator<
	* 3. the type of the object must be Cpp17CopyAssignable
	*/
	template <typename T>
	void insertion_sort(T begin, T end)
	{
		// left subseq: [begin, leftBound) (the empty set if leftBound = begin)
		// right subseq: [leftBound, end)

		if (end - begin >= 2)
		{
			T leftBound = begin;
			for (; leftBound != end; ++leftBound) // after the loop the entire list will become left seq and will thus be sorted.
			{
				auto firstRight = *leftBound;

				// insert firstRight into the correct position
				if (begin != leftBound) // left subseq not empty
				{
					for (auto i = leftBound - 1; /* will decide when to break in the loop */ ; --i) // iterate through left subseq
					{
						if (*i < firstRight) // insert the element after i if *i is the first that < it
						{
							// move all elements in range (i,leftBound) one step forward (thus *leftBound is overwritten)
							if (leftBound != i+1) // (i,leftBound) is not empty
							{
								for (auto j = leftBound; j != i+1; --j)
								{
									*j = *(j - 1); // Cpp17CopyAssignable
								}
							}
							// move firstRight to *(i+1)
							*(i + 1) = firstRight;

							break;
						}
						else if (begin == i) // or if we reached begin and *begin also > it (then all of the left seq > it), then we insert it at begin
						{
							// move all elements in range [begin,leftBound) one step forward (thus *leftBound is overwritten)
							if (leftBound != begin) // [begin,leftBound) is not empty
							{
								for (auto j = leftBound; j != begin; --j)
								{
									*j = *(j - 1); // Cpp17CopyAssignable
								}
							}
							// move firstRight to *begin
							*begin = firstRight;
							break;
						}
					}
				}
			}
		}
	}

	/*
	* Sorting by inserting elements from right subseq to left subseq (insertion here causes swapping some elements in a range)
	*
	* Invariant:
	* left seq is sorted in ascending order
	*
	* Requirements:
	* 1. T is a list that contains the elements to be sorted
	* 2. T has insert(pos, val) that inserts val between pos - 1 and pos
	* 3. T has remove(pos) that removes the element at pos
	* 4. The type of the object must have an operator<
	* 5. An iterator of T must be a LegacyRandomAccessIterator and a LegacyOutputIterator
	*/
	template <typename T>
	void insertion_sort(T& list)
	{
		// left subseq: [begin, leftBound) (the empty set if leftBound = begin)
		// right subseq: [leftBound, end)

		auto begin = list.begin(), end = list.end();

		if (end - begin >= 2)
		{
			T leftBound = begin;
			for (; leftBound != end; ++leftBound) // after the loop the entire list will become left seq and will thus be sorted.
			{
				auto firstRight = *leftBound;

				// insert firstRight into the correct position
				if (begin != leftBound) // left subseq not empty
				{
					for (auto i = leftBound - 1; /* will decide when to break in the loop */; --i) // iterate through left subseq
					{
						if (*i < firstRight) // insert the element after i if *i is the first that < it
						{
							list.erase(leftBound);
							list.insert(i + 1, firstRight); // + 1 because insert() inserts BEFORE pos
							break;
						}
						else if (begin == i) // or if we reached begin and *begin also > it (then all of the left seq > it), then we insert it at begin
						{
							list.erase(leftBound);
							list.insert(begin, firstRight); // this time we don't + 1 because we need to insert it before all left seq
							break;
						}
					}
				}
			}
		}
	}

	/*
	* Selects the minimal element of the right subsequence and arranges it on the left
	*
	* Invariant:
	* 1. all elements in the left subsequence are lower bounds of the right subseq
	* 2. left subseq is sorted in ascending order
	* 
	* Requirements:
	* 1. T is a LegacyRandomAccessIterator and a LegacyOutputIterator
	* 2. the type of the object must have an operator<
	* 3. the type of the object must be Cpp17CopyConstructible, Cpp17CopyAssignable
	*/
	template <typename T>
	void selection_sort(T begin, T end)
	{
		// left subseq: [begin, leftBound) (the empty set if leftBound = begin)
		// right subseq: [leftBound, end)

		if (end - begin >= 2)
		{
			T leftBound = begin;
			for (; leftBound != end; ++leftBound) // each iteration forwards leftBound by 1, and increases left subseq by 1
			{
				T rightMinIndex = leftBound; // we have to remember the index to put it on the left
				auto rightMin = *rightMinIndex;
				for (auto i = leftBound; i != end; ++i) // find the minimal of right subseq by iteration
				{
					if (*i < rightMin)
					{
						rightMin = *i;
						rightMinIndex = i;
					}
				}

				// now rightMinIndex is established, put the minimal element on the left
				// (we do this here by swapping it with the element on leftBound)
				if (rightMinIndex != leftBound)
				{
					auto temp = *rightMinIndex;
					*rightMinIndex = *leftBound;
					*leftBound = temp;
				}
			}
		}
	}

	/*
	* Overloading to selection_sort
	* Equivalent to selection_sort(list.begin(), list.end());
	*/
	template <typename T>
	void selection_sort(T & list)
	{
		selection_sort(list.begin(), list.end());
	}

	/*
	* Merges two sublists that are divided in merge_sort below
	*/
	template <typename T>
	void merge(T begin, T mid, T end)
	{
		// copy right and left
		ghl::vector<std::remove_reference_t<decltype(*begin)>> lCpy(begin, mid);
		ghl::vector<std::remove_reference_t<decltype(*begin)>> rCpy(mid, end);

		auto lIter = lCpy.begin(), rIter = rCpy.begin();
		auto lEnd = lCpy.end(), rEnd = rCpy.end();
		T i = begin;

		for (; lIter != lEnd && rIter != rEnd; ++i) // loop until either of the two is empty
		{
			if (*lIter < *rIter) // the miminal of left is the smallest, move it to the next place
			{
				*i = *lIter;
				++lIter;
			}
			else // the miminal of right is the smallest, move it to the next place
			{
				*i = *rIter;
				++rIter;
			}
		}

		if (lIter == lEnd) // lCpy is empty
		{
			// copy the remaining of rCpy
			while (rIter != rEnd)
			{
				*i = *rIter;
				++i;
				++rIter;
			}
		}
		else // rCpy is empty
		{
			// copy the remaining of lCpy
			while (lIter != lEnd)
			{
				*i = *lIter;
				++i;
				++lIter;
			}
		}
	}

	/*
	* Recursively merge sorts two consecutive sublists in ascending order.
	* The two sublists are [begin, mid) and [mid, end), respectively.
	*
	* If any of the sublist's size is 1 (i.e. < 2), then the sublist is trivially sorted, and that's when the recursion ends.
	* 
	* Requirements:
	* 1. T is a LegacyRandomAccessIterator and a LegacyOutputIterator
	* 2. the type of the object must have an operator<
	* 3. the type of the object must be Cpp17CopyConstructible, Cpp17CopyAssignable, Cpp17MoveConstructible (required by ghl::vector)
	*/
	template <typename T>
	void merge_sort(T begin, T mid, T end)
	{
		auto lSize = mid - begin, rSize = end - mid;
		if (lSize >= 2)
		{
			// if lSize is even, then the two are evenly divided
			// otherwise, left sublist is one element less than right
			merge_sort(begin, begin + lSize / 2, mid);
		}
		if (rSize >= 2)
		{
			// if rSize is even, then the two are evenly divided
			// otherwise, left sublist is one element less than right
			merge_sort(mid, mid + rSize / 2, end);
		}

		merge(begin, mid, end);
	}

	/*
	* Overloading to merge_sort,
	* equivalent to merge_sort(list.begin(), list.begin() + (list.end() - list.begin()) / 2, list.end())
	* iff list.end() - list.begin() >= 2
	*/
	template <typename T>
	void merge_sort(T& list)
	{
		if (list.end() - list.begin() >= 2)
		{
			merge_sort(list.begin(), list.begin() + (list.end() - list.begin()) / 2, list.end());
		}
	}
}