/*
* This file contains the implementation of the algorithms taught in INT102 about graphs
*/

#pragma once

#include "../data_structures/graph.h"
#include "../data_structures/queue.h"

// for inf
#include <limits>

namespace ghl
{
	// v.color is b_visited, where we ignore grey so it can be contained in a boolean
	// v.d is d
	// for v.pi, we will put this bfs_attr in an edge, where edge::left is v, edge::right is v.pi
	struct bfs_attr
	{
		bfs_attr() {}
		bfs_attr(bool b, unsigned in_d) : b_visited(b), d(in_d) {}

		bool b_visited = false;
		unsigned d = inf;

		bool operator<=(const bfs_attr& right) const { return d <= right.d; }

		static constexpr auto inf = std::numeric_limits<decltype(d)>::max();
	};

	template <typename T>
	using bfs_edge = edge<T, bfs_attr>;

	/*
	* Performs breadth first search on graph with traversal_functor and base_vertex
	* 
	* G: an instantiation of ghl::graph
	* F: a callable object whose operator() takes an argument of G::weak_ref_t.
	* ID: the id of the base vertex used to do this traversal
	*/
	template <typename G, typename F, typename ID, typename E = bfs_edge<typename G::obj_t>>
	void breadth_first_search(G& graph, F search_functor, ID base_vertex)
	{
		
	}

	/*
	* Performs Prim's algorithm on graph (assumed to be simple and connected) with base_vertex, 
	* whose output is written to tree (assumed to be empty when passed in)
	* 
	* G: an instantiation of ghl::graph, which should be a simple graph
	* ID: the id of the base vertex used to do this traversal
	*/
	template <typename G, typename ID, typename E = typename G::edge_t>
	void prims_algorithm(const G& graph, G& tree, ID base_vertex)
	{
		// v = included vertices = {}
		ghl::list<typename G::weak_ref_t> v;

		// u = unincluded vertices = G.V
		ghl::list<typename G::weak_ref_t> u; graph.get_all_vertices(u);

		// we use edge as the structure to store the key(v) and pi(v) for a vertex v
		// E::left = v, E::right = pi(v), E::weight = key(v)

		// the priority queue
		ghl::priority_queue<E> q;

		// put all edges in it and set every key = inf.
		// then, add base(nil, 0) to it
		ghl::list<E> edges; graph.get_all_edges(edges);
		for (E e : edges)
		{
			e.weight = std::numeric_limits<decltype(e.weight)>::infinity;
			q.push(e);
		}

		auto base_v = graph.find_vertex(base_vertex);
		q.push(E(base_v, typename G::weak_ref_t() /*nil*/, 0));

		E e;
		while (!q.empty())
		{
			// the edge with the smallest weight
			// this edge will never be connecting vertices that are both in v,
			// because if there existed such an edge h, and let h.left = a, l.right = b,
			// then when the first time a was included in v, we would have had two conditions,
			//		1. b had already been in v
			//		2. b was not in v
			// if it was 1., then at the first time b had been included, a.key and a.pi could be updated.
			//		i. if it was updated (i.e. when h.weight was minimal among the others), then a.key = h.weight and a.pi = b
			//		ii. if it was not,
			e = q.pop();
			// undirected graph maintain edges in both directions, so we just select e.left regardless of it being directed or not
			auto ver = e.left;
			
			
		}
	}
}
