/*
* This file contains the implementation of the algorithms taught in INT102 about graphs
*/

#pragma once

#include "../data_structures/graph.h"

namespace ghl
{
	/*
	* Performs breadth first traversal on graph with traversal_functor and base_vertex
	* 
	* G: an instantiation of ghl::graph
	* F: a callable object whose operator() takes an argument of G::weak_ref_t.
	* ID: the id of the base vertex used to do this traversal
	*/
	template <typename G, typename F, typename ID>
	void breadth_first_traversal(G& graph, F traversal_functor, ID base_vertex)
	{
		auto base_ref = graph.find_vertex(base_vertex);

		if (base_ref.valid()) // found the vertex
		{
			ghl::list<typename G::weak_ref_t> traversal_queue{ base_ref };

			while (!traversal_queue.empty())
			{
				// traverse the front element
				auto ref = traversal_queue.front();
				traversal_functor(ref);

				// if it is not isolated, put all vertices directedly connected to it to the queue
				if (!ref.observe().is_isolated())
				{
					for (auto iter = graph.get_directly_connected_edges(); iter.has_next(); ++iter)
					{
						traversal_queue.insert_back((*(iter).right));
					}
				}

				// remove it from the queue
				traversal_queue.remove_front();
			}
		}
	}

	/*
	* Performs Prim's algorithm on graph (assumed to be simple and connected) with base_vertex, 
	* whose output is written to tree (assumed to be empty when passed in)
	* 
	* G: an instantiation of ghl::graph
	* ID: the id of the base vertex used to do this traversal
	*/
	template <typename G, typename ID>
	void prims_algorithm(G& graph, G& tree, ID base_vertex)
	{
		// V = included vertices = {}
		ghl::list<typename G::weak_ref_t> V;

		// U = unincluded vertices = G.V
		ghl::list<typename G::weak_ref_t> U; graph.get_all_vertices(U);

		
	}
}
