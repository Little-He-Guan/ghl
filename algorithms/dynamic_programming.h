#pragma once

#include <typeinfo>

/*
* Contains DP algorithms taught in INT102
*/

namespace ghl
{
	/*
	* @returns the n^th number of the fibnacci sequence
	*/
	uint64_t fib_dp(unsigned n);

	// forward declaration
	template <typename T>
	class vector;

	/*
	* Solves the assembly line problem by using dp and stores the steps in argument steps
	*
	* @param first where first[i] = the time cost of processing using the (i)^th assembly machine in the top line
	* @param second where second[i] = the time cost of processing using the (i)^th assembly machine in the bottom line
	* @param fts where fts[i] = the time cost of transferring from the i^th machine in the top line to the (i+1)^th machine in the bottom line
	* @param stf where stf[i] = the time cost of transferring from the i^th machine in the bottom line to the (i+1)^th machine in the top line
	* @param steps where step[i] = false means the (i)^th step falls on the top assembly line, and true means the step falls on the bottom one. (assumed to be empty when passed in)
	*
	* @returns the total time spent applying the optimal solution
	*/
	uint64_t assembly_line_dp
	(
		const ghl::vector<unsigned>& first, const ghl::vector<unsigned>& second,
		const ghl::vector<unsigned>& fts, const ghl::vector<unsigned>& stf,
		ghl::vector<bool>& steps
	);

	enum class subsequence_direction : char
	{
		top_left,
		top,
		left
	};

	/*
	* Solves the longest common subsequence problem using dynamic programming
	* 
	* @param first the first sequence
	* @param second the second sequence
	* @param directions the directions that one LCS goes during the construction
	* 
	* @returns the length of LCS(first,second)
	*/
	int longest_common_subsequence
	(
		const ghl::vector<char> & first, const ghl::vector<char> & second,
		ghl::vector<ghl::vector<subsequence_direction>> & directions
	);

	/*
	* Prints the LCS calculated by the above function
	*
	* @param first the first sequence
	* @param directions the directions that the calculated LCS goes during the construction
	* @param i the index of the current place in first
	* @param j the index of the current place in second
	*/
	void print_longest_common_subsequence
	(
		const ghl::vector<char>& first,
		const ghl::vector<ghl::vector<subsequence_direction>>& directions,
		int i, int j
	);

	/*
	* Solves the best alignment problem using dp
	* 
	* @param d gap_penalty
	* @param matrix score matrix
	* @param first the sequence that is to be matched against
	* @param second the sequence to match against first
	* @param directions the directions that the calculated alignment goes during the construction
	* 
	* @returns the score of the optimal match
	*/
	int best_alignment
	(
		int d,
		const ghl::vector<ghl::vector<int>> &matrix,
		const ghl::vector<int>& first, const ghl::vector<int>& second,
		ghl::vector<ghl::vector<ghl::subsequence_direction>>& directions
	);
}