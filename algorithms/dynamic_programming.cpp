#include "dynamic_programming.h"

#include "../data_structures/vector.h"

uint64_t ghl::fib_dp(unsigned n)
{
	static ghl::vector<uint64_t> sequence{ 0,1,1 /*base case: 0 <= n <= 2*/ };

	if (n >= sequence.size()) // result not calculated
	{
		auto prev_size = sequence.size();
		// set capacity and size all to n+1
		sequence.resize((uint64_t)n + 1);
		sequence.increase_size((uint64_t)n + 1);

		// store all newly made calculations
		for (auto i = prev_size; i != sequence.size(); ++i)
		{
			sequence[i] = sequence[i - 1] + sequence[i - 2];
		}
	}

	return sequence[n];
}

uint64_t ghl::assembly_line_dp
(
	const ghl::vector<unsigned>& first, const ghl::vector<unsigned>& second,
	const ghl::vector<unsigned>& fts, const ghl::vector<unsigned>& stf,
	ghl::vector<bool>& steps
)
{
	auto n = first.size();

	// the shortest cost to go to first[i] or second[i]
	ghl::vector<uint64_t> shortest_to_first(n), shortest_to_second(n);
	// the optimal steps to reach the i^th machine for first and second
	ghl::vector<bool> steps_f(n), steps_s(n);

	// base case
	shortest_to_first.push_back(first[0]); shortest_to_second.push_back(second[0]);
	steps_f.push_back(false); steps_s.push_back(true);

	/*
	* Loop invariant: for all x < i
	* shortest_to_first[x]  = the cost of the shortest path to machine i at top line
	* shortest_to_second[x] = the cost of the shortest path to machine i at bot line
	* from steps_f[0] to steps_f[i-1] is all the steps that form the shortest path to machine i at top line
	* from steps_s[0] to steps_s[i-1] is all the steps that form the shortest path to machine i at bot line
	*/
	for (int i = 1; i < n; ++i)
	{
		// decide which is the shortest way to go to first[i]
		// only has two ways: from first[i-1] or from second[i-1]
		// the shortest way is thus from 
		// 1. the shortest way to first[i-1] + first[i-1] to first[i] (cost of first[i])
		// 2. the shortest way to second[i-1] + second[i-1] to first[i] (cost of transfer + first[i])

		// decide which is the shortest way to go to second[i]
		// only has two ways: from first[i-1] or from second[i-1]
		// the shortest way is thus from 
		// a. the shortest way to first[i-1] + first[i-1] to second[i] (cost of transfer + second[i])
		// b. the shortest way to second[i-1] + second[i] (second[i])

		// let f be the steps of the shortest way to first[i-1], and
		// s be the steps ... to second[i-1]
		// condition 1b (no transfer):
		//		steps_f becomes f + false
		//		steps_s becomes s + true
		// condition 1a
		//		steps_f becomes f + false
		//		steps_s becomes f + true
		// condition 2a
		//		steps_f becomes s + false
		//		steps_s becomes f + true
		// condition 2b
		//		steps_f becomes s + false
		//		steps_s becomes s + true

		// Only condition 2a consists of a swap of f and s, (which are previously stored in steps_f and steps_s, respectively)
		// which requires at least another temp variable

		auto way1_f = shortest_to_first[i - 1] + first[i], way2_f = shortest_to_second[i - 1] + stf[i - 1] + first[i];
		auto way1_s = shortest_to_first[i - 1] + fts[i - 1] + second[i], way2_s = shortest_to_second[i - 1] + second[i];
	
		bool b12 = way1_f <= way2_f;
		bool bab = way1_s <= way2_s;

		if (b12 && bab) // 1a
		{
			steps_f.push_back(false); // steps_f becomes f + false

			for (int j = 0; j < i; ++j) // copy f to steps_s
			{
				steps_s[j] = steps_f[j];
			}
			steps_s.push_back(true); // steps_s becomes f + true
		
			shortest_to_first.push_back(way1_f); // shortest_to_first[i] = way1_f
			shortest_to_second.push_back(way1_s); // shortest_to_second[i] = way1_s
		}
		else if (b12 && (!bab)) // 1b
		{
			steps_f.push_back(false); // steps_f becomes f + false
			steps_s.push_back(true); // steps_s becomes s + true

			shortest_to_first.push_back(way1_f); // shortest_to_first[i] = way1_f
			shortest_to_second.push_back(way2_s); // shortest_to_second[i] = way2_s
		}
		else if ((!b12) && bab) // 2a
		{
			bool temp;
			for (int j = 0; j < i; ++j) // swap f with s
			{
				temp = steps_s[j];
				steps_s[j] = steps_f[j];
				steps_f[j] = temp;
			}

			steps_f.push_back(false); // steps_f becomes s + false
			steps_s.push_back(true); // steps_s becomes f + true

			shortest_to_first.push_back(way2_f); // shortest_to_first[i] = way2_f
			shortest_to_second.push_back(way1_s); // shortest_to_second[i] = way1_s
		}
		else // 2b
		{
			steps_s.push_back(true); // steps_s becomes s + true
			
			for (int j = 0; j < i; ++j) // copy s to steps_f
			{
				steps_f[j] = steps_s[j];
			}
			steps_f.push_back(false); // steps_f becomes s + false;

			shortest_to_first.push_back(way2_f); // shortest_to_first[i] = way2_f
			shortest_to_second.push_back(way2_s); // shortest_to_second[i] = way2_s
		}
	}

	// now we finished, decide which of the two paths is optimal
	if (shortest_to_first[n - 1] <= shortest_to_second[n - 1])
	{
		steps = steps_f;
		return shortest_to_first[n - 1];
	}
	else
	{
		steps = steps_s;
		return shortest_to_second[n - 1];
	}
}
