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
		ghl::vector<bool> & steps
	);
}