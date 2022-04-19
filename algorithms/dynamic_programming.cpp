#include "dynamic_programming.h"

#include "../data_structures/vector.h"

#include <iostream>

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

int ghl::longest_common_subsequence
(
	const ghl::vector<char>& first,
	const ghl::vector<char>& second,
	ghl::vector<ghl::vector<ghl::subsequence_direction>>& directions
)
{
	// length_matrix[i][j] = length of LCS(first_i, second_j)
	ghl::vector<ghl::vector<int>> length_matrix;

	auto m = first.size(), n = second.size();

	length_matrix.resize(m);
	for (int i = 0; i != m; ++i)
	{
		// construct the columns that are of size n
		length_matrix.emplace_back(n);
		length_matrix.increase_size(n);
	}

	directions.resize(m);
	for (int i = 0; i != m; ++i)
	{
		// construct the columns that are of size n 
		directions.emplace_back(n);
		directions[i].increase_size(n);
	}

	// now we calculate the lengths and directions

	// base case:
	for (int i = 0; i != m; ++i)
	{
		length_matrix[i][0] = 0;
	}
	for (int j = 0; j != n; ++j)
	{
		length_matrix[0][j] = 0;
	}

	// the recursive steps
	for (int i = 1; i != m; ++i)
	{
		for (int j = 1; j != n; ++j)
		{
			// at this time, [i-1][any], [i][j-1] are all calculated

			if (first[i] == second[j]) // length is length_matrix[i - 1][j - 1] + 1;
			{
				length_matrix[i][j] = length_matrix[i - 1][j - 1] + 1;
				directions[i][j] = ghl::subsequence_direction::top_left;
			}
			else
			{
				// length is the max of the two
				if (length_matrix[i - 1][j] >= length_matrix[i][j - 1])
				{
					length_matrix[i][j] = length_matrix[i - 1][j];
					directions[i][j] = ghl::subsequence_direction::top;
				}
				else
				{
					length_matrix[i][j] = length_matrix[i][j - 1];
					directions[i][j] = ghl::subsequence_direction::left;
				}
			}
		}
	}

	return length_matrix[m - 1][n - 1];
}

void ghl::print_longest_common_subsequence
(
	const ghl::vector<char>& first,
	const ghl::vector<ghl::vector<subsequence_direction>>& directions,
	int i, int j
)
{
	if (i <= -1 || j <= -1) return;

	switch (directions[i][j])
	{
	case ghl::subsequence_direction::top_left:
		print_longest_common_subsequence(first, directions, i - 1, j - 1);
		std::cout << first[i];
		break;
	case ghl::subsequence_direction::top:
		print_longest_common_subsequence(first, directions, i-1, j);
		break;
	case ghl::subsequence_direction::left:
		print_longest_common_subsequence(first, directions, i, j-1);
		break;
	}
}

int ghl::best_alignment
(
	int d,
	const ghl::vector<ghl::vector<int>>& matrix,
	const ghl::vector<int>& first, const ghl::vector<int>& second,
	ghl::vector<ghl::vector<ghl::subsequence_direction>>& directions
)
{
	ghl::vector<ghl::vector<int>> scores;

	auto m = first.size(), n = second.size();

	scores.resize(m+1);
	for (int i = 0; i != m+1; ++i)
	{
		// construct the columns that are of size n+1
		scores[i].emplace_back(n + 1);
		scores[i].increase_size(n + 1);
	}

	directions.resize(m+1);
	for (int i = 0; i != m+1; ++i)
	{
		// construct the columns that are of size n +1 
		directions.emplace_back(n+1);
		directions[i].increase_size(n+1);
	}

	// now we calculate the lengths and directions

	// base case:
	{
		for (int i = 0; i != m + 1; ++i)
		{
			scores[i][0] = i * d;
			if (0 == i)
			{
				directions[i][0] = ghl::subsequence_direction::top_left;
			}
			else
			{
				directions[i][0] = ghl::subsequence_direction::top;
			}
		}
		for (int j = 0; j != n + 1; ++j)
		{
			scores[0][j] = j * d;
			if (0 == j)
			{
				directions[0][j] = ghl::subsequence_direction::top_left;
			}
			else
			{
				directions[0][j] = ghl::subsequence_direction::left;
			}
		}
	}

	// recursive steps:

	{
		int top_left_score, top_score, left_score;

		for (int i = 1; i != m + 1; ++i)
		{
			for (int j = 1; j != n + 1; ++j)
			{
				// at this time, [i-1][any], [i][j-1] are all calculated

				top_left_score = scores[i - 1][j - 1] + matrix[i - 1][j - 1];
				top_score = scores[i][j - 1] + d;
				left_score = scores[i - 1][j] + d;

				// find the biggest one among the three
				int biggest_index = -1, max = 0;
				if (top_score >= left_score)
				{
					if (top_left_score >= top_score)
					{
						biggest_index = 1;
						max = top_left_score;
					}
					else
					{
						biggest_index = 2;
						max = top_score;
					}
				}
				else
				{
					if (top_left_score >= left_score)
					{
						biggest_index = 1;
						max = top_left_score;
					}
					else
					{
						biggest_index = 3;
						max = left_score;
					}
				}

				// update the matrices
				scores[i][j] = max;
				switch (biggest_index)
				{
				case 1:
					directions[i][j] = ghl::subsequence_direction::top_left;
					break;
				case 2:
					directions[i][j] = ghl::subsequence_direction::top;
					break;
				case 3:
					directions[i][j] = ghl::subsequence_direction::left;
					break;
				}
			}
		}
	}

	return 0;
}