// tests for class adj_list_graph_ds

#include "../data_structures/graph.h"
#include "../unit_test/test_unit.h"

#include <iostream>

namespace ghl
{
	class adj_list_graph_ds_tester
	{
	public:
		template <typename T>
		static auto& get_map(ghl::adj_list_graph_ds<T>& g) { return g.vertices_and_lists; }
	};
}

/*
* Assumes find_vertex, has_edge, get_edge works fine because they are too simple
*/

DEFINE_TEST_CASE(test_adj_graph_ctor)

	ghl::adj_list_graph_ds<int> g;
	ASSERT_TRUE(g.empty(), "expected to get an empty graph")

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_adj_graph_add_vertex)

	ghl::adj_list_graph_ds<int> g;
	auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

	// adding vertex to an empty graph
	{
		auto a_ref = g.add_vertex("a", 3);

		ASSERT_EQUALS(1, g.num_vertices(), "expected to have the vertex")
		ASSERT_EQUALS(0, g.num_edges(), "expected to have no edges introduced")

		const auto &v = map.find("a")->first;

		ASSERT_EQUALS(v, a_ref.observe(), "expected to have the ref to the added vertex")
		ASSERT_EQUALS(3, *(a_ref.observe().obj), "expected to have the value contained in the vertex correct")
	}

	// adding vertex to a non-empty graph
	{
		auto b_ref = g.add_vertex("b", 5);

		ASSERT_EQUALS(2, g.num_vertices(), "expected to have the vertex")
		ASSERT_EQUALS(0, g.num_edges(), "expected to have no edges introduced")

		const auto &v = map.find("b")->first;

		ASSERT_EQUALS(v, b_ref.observe(), "expected to have the ref to the added vertex")
		ASSERT_EQUALS(5, *(b_ref.observe().obj), "expected to have the value contained in the vertex correct")
	}

	// adding an existing vertex to a non-empty graph
	{
		auto a_ref = g.add_vertex("a", 9);

		ASSERT_EQUALS(2, g.num_vertices(), "expected to have the number unchanged")
		ASSERT_EQUALS(0, g.num_edges(), "expected to have no edges introduced")

		const auto &v = map.find("a")->first;

		ASSERT_EQUALS(v, a_ref.observe(), "expected to have the ref to the original vertex")
		ASSERT_EQUALS(3, *(a_ref.observe().obj), "expected to have the original vertex, not the new one")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_adj_graph_remove_vertex)

	ghl::adj_list_graph_ds<int> g;
	auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

	// removing a vertex from an empty graph
	{
		g.remove_vertex("a");

		ASSERT_EQUALS(0, g.num_vertices(), "expected to do nothing")
		ASSERT_EQUALS(0, g.num_edges(), "expected to do nothing")
	}

	// removing an existing vertex from a graph
	{
		g.add_vertex("b", 5);
		g.add_vertex("a", 3);
		g.remove_vertex("a");

		ASSERT_EQUALS(1, g.num_vertices(), "expected to remove the vertex")
		ASSERT_EQUALS(0, g.num_edges(), "expected to do nothing about edges")

		ASSERT_TRUE(map.end() == map.find("a"), "expected to remove the vertex")
		ASSERT_TRUE(map.end() != map.find("b"), "expected to not affect other vertices")
	}

	// removing a non-existing vertex from a graph
	{
		g.remove_vertex("c");

		ASSERT_EQUALS(1, g.num_vertices(), "expected to have the number unchanged")
		ASSERT_EQUALS(0, g.num_edges(), "expected to do nothing about edges")

		ASSERT_TRUE(map.end() != map.find("b"), "expected to not affect other vertices")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_adj_graph_remove_vertex_with_edges) // we shall see if edges and degs are adjusted properly

	// removing an existing vertex that has edges connected to it from a graph (undirected)
	{
		ghl::adj_list_graph_ds<int> g(true); // undirected graph
		auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

		g.add_vertex("b", 5);
		g.add_vertex("a", 3);
		g.add_vertex("c", 4);

		auto& a = const_cast<ghl::vertex<int>&>(map.find("a")->first);
		auto& b = const_cast<ghl::vertex<int>&>(map.find("b")->first);
		auto& c = const_cast<ghl::vertex<int>&>(map.find("c")->first);

		// let the edges be {a,c},{a,b},{b,c}
		a.deg += 2; b.deg += 2; c.deg += 2;
		map[a].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(b));
		map[b].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(a));
		map[a].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(c));
		map[b].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(c));
		map[c].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(b));
		map[c].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(a));

		g.remove_vertex("c");

		ASSERT_EQUALS(2, g.num_vertices(), "expected to remove a vertex")
		ASSERT_EQUALS(1, g.num_edges(), "expected to remove 2 edges")

		ASSERT_TRUE(map.find("c") == map.end(), "expect to remove the vertex");

		ASSERT_TRUE(g.has_edge("a", "b"), "expected to not affect other edges");

		ASSERT_EQUALS(1, a.deg, "expected to have the deg right")
		ASSERT_EQUALS(1, b.deg, "expected to have the deg right")
	}

	// removing an existing vertex that has edges connected to it from a graph (directed)
	{
		ghl::adj_list_graph_ds<int> g(false); // undirected graph
		auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

		g.add_vertex("b", 5);
		g.add_vertex("a", 3);
		g.add_vertex("c", 4);

		auto& a = const_cast<ghl::vertex<int>&>(map.find("a")->first);
		auto& b = const_cast<ghl::vertex<int>&>(map.find("b")->first);
		auto& c = const_cast<ghl::vertex<int>&>(map.find("c")->first);

		// let the edges be (a,b), (a,c), (b,a), (b,c), (c,a), (c,b)
		a.indeg += 2; b.indeg += 2; c.indeg += 2;
		a.outdeg += 2; b.outdeg += 2; c.outdeg += 2;
		map[a].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(b));
		map[b].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(a));
		map[a].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(c));
		map[b].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(c));
		map[c].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(b));
		map[c].insert_back(ghl::adj_list_graph_ds<int>::vertex_ref(a));

		g.remove_vertex("c");

		ASSERT_EQUALS(2, g.num_vertices(), "expected to remove a vertex")
		ASSERT_EQUALS(2, g.num_edges(), "expected to remove 4 edges")

		ASSERT_TRUE(map.find("c") == map.end(), "expect to remove the vertex");

		ASSERT_TRUE(g.has_edge("a", "b") && g.has_edge("b", "a"), "expected to not affect other edges");

		ASSERT_EQUALS(1, a.indeg, "expected to have the deg right")
		ASSERT_EQUALS(1, b.indeg, "expected to have the deg right")
		ASSERT_EQUALS(1, a.outdeg, "expected to have the deg right")
		ASSERT_EQUALS(1, b.outdeg, "expected to have the deg right")
	}
ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_adj_graph_add_edge)

	// adding an edge with non-existing vertices
	{
		ghl::adj_list_graph_ds<int> g;
		auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

		// both non-existing
		ASSERT_FALSE(g.add_edge("a", "b"), "expected to fail to add the edge");

		g.add_vertex("a", 1);

		// only right non-existing
		ASSERT_FALSE(g.add_edge("a", "b"), "expected to fail to add the edge");

		g.remove_vertex("a");
		g.add_vertex("b");

		// only left non-existing
		ASSERT_FALSE(g.add_edge("a", "b"), "expected to fail to add the edge");
	}

	// adding an edge with existing vertices (undirected)
	{
		ghl::adj_list_graph_ds<int> g(true); // undirected
		auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

		g.add_vertex("b", 5);
		g.add_vertex("a", 3);
		g.add_vertex("c", 4);

		auto& a = const_cast<ghl::vertex<int>&>(map.find("a")->first);
		auto& b = const_cast<ghl::vertex<int>&>(map.find("b")->first);
		auto& c = const_cast<ghl::vertex<int>&>(map.find("c")->first);

		g.add_edge("a", "b", .5f);

		ASSERT_EQUALS(1, g.num_edges(), "expected to add an edge")

		ASSERT_EQUALS(1, a.deg, "expected to have the deg right")
		ASSERT_EQUALS(1, b.deg, "expected to have the deg right")

		ASSERT_TRUE(g.has_edge("a", "b") && g.has_edge("b", "a"), "expected to have exactly this edge")
		ASSERT_EQUALS(.5f, g.get_edge("a", "b").weight, "expected to have the weight correct")
	}

	// adding an edge with existing vertices (directed)
	{
		ghl::adj_list_graph_ds<int> g(false); // directed
		auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

		g.add_vertex("b", 5);
		g.add_vertex("a", 3);
		g.add_vertex("c", 4);

		auto& a = const_cast<ghl::vertex<int>&>(map.find("a")->first);
		auto& b = const_cast<ghl::vertex<int>&>(map.find("b")->first);
		auto& c = const_cast<ghl::vertex<int>&>(map.find("c")->first);

		g.add_edge("a", "b", .5f);

		ASSERT_EQUALS(1, g.num_edges(), "expected to add an edge")

		ASSERT_EQUALS(1, a.outdeg, "expected to have the deg right")
		ASSERT_EQUALS(1, b.indeg, "expected to have the deg right")

		ASSERT_TRUE(g.has_edge("a", "b"), "expected to have exactly this edge")
		ASSERT_EQUALS(.5f, g.get_edge("a", "b").weight, "expected to have the weight correct")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_adj_graph_remove_edge)

	// removing an egde with non-existing vertices (undirected)
	{
		ghl::adj_list_graph_ds<int> g;
		auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

		g.add_vertex("b", 5);
		g.add_vertex("a", 3);
		g.add_vertex("c", 4);

		auto& a = const_cast<ghl::vertex<int>&>(map.find("a")->first);
		auto& b = const_cast<ghl::vertex<int>&>(map.find("b")->first);
		auto& c = const_cast<ghl::vertex<int>&>(map.find("c")->first);

		g.add_edge("a", "b", .2f);

		ASSERT_FALSE(g.remove_edge("a", "c"), "expected to fail to remove the edge");

		ASSERT_TRUE(g.has_edge("a", "b") && g.has_edge("b", "a"), "expected to not affect other edges")
		ASSERT_EQUALS(1, g.num_edges(), "expected to not affect other edges")
		ASSERT_EQUALS(3, g.num_vertices(), "expected to not affect vertices")

		ASSERT_EQUALS(1, a.deg, "expected to not affect deg")
		ASSERT_EQUALS(1, b.deg, "expected to not affect deg")
		ASSERT_EQUALS(0, c.deg, "expected to not affect deg")
	}

	// removing an egde with non-existing vertices (directed)
	{
		ghl::adj_list_graph_ds<int> g(false);
		auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

		g.add_vertex("b", 5);
		g.add_vertex("a", 3);
		g.add_vertex("c", 4);

		auto& a = const_cast<ghl::vertex<int>&>(map.find("a")->first);
		auto& b = const_cast<ghl::vertex<int>&>(map.find("b")->first);
		auto& c = const_cast<ghl::vertex<int>&>(map.find("c")->first);

		g.add_edge("a", "b", .2f);

		ASSERT_FALSE(g.remove_edge("a", "c"), "expected to fail to remove the edge");

		ASSERT_TRUE(g.has_edge("a", "b"), "expected to not affect other edges")
		ASSERT_EQUALS(1, g.num_edges(), "expected to not affect other edges")
		ASSERT_EQUALS(3, g.num_vertices(), "expected to not affect vertices")

		ASSERT_EQUALS(1, a.outdeg, "expected to not affect deg")
		ASSERT_EQUALS(1, b.indeg, "expected to not affect deg")
		ASSERT_EQUALS(0, c.deg, "expected to not affect deg")
	}

	// removing an existing edge (undirected)
	{
		ghl::adj_list_graph_ds<int> g;
		auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

		g.add_vertex("b", 5);
		g.add_vertex("a", 3);
		g.add_vertex("c", 4);

		auto& a = const_cast<ghl::vertex<int>&>(map.find("a")->first);
		auto& a_list = map.find("a")->second;
		auto& b = const_cast<ghl::vertex<int>&>(map.find("b")->first);
		auto& c = const_cast<ghl::vertex<int>&>(map.find("c")->first);

		g.add_edge("a", "c", .2f);
		g.add_edge("a", "b", .3f);
		g.add_edge("c", "b", .4f);

		ASSERT_TRUE(g.remove_edge("a", "b"), "expected to successfully remove the edge");

		ASSERT_TRUE(g.has_edge("a", "c") && g.has_edge("c", "a"), "expected to not affect other edges")
		ASSERT_TRUE(g.has_edge("c", "b") && g.has_edge("b", "c"), "expected to not affect other edges")
		ASSERT_EQUALS(2, g.num_edges(), "expected to not affect other edges")
		ASSERT_EQUALS(3, g.num_vertices(), "expected to not affect vertices")

		ASSERT_EQUALS(1, a.deg, "expected to got deg right")
		ASSERT_EQUALS(1, b.deg, "expected to got deg right")
		ASSERT_EQUALS(2, c.deg, "expected to got deg right")
	}

	// removing an existing edge (directed)
	{
		ghl::adj_list_graph_ds<int> g(false);
		auto& map = ghl::adj_list_graph_ds_tester::get_map(g);

		g.add_vertex("b", 5);
		g.add_vertex("a", 3);
		g.add_vertex("c", 4);

		auto& a = const_cast<ghl::vertex<int>&>(map.find("a")->first);
		auto& b = const_cast<ghl::vertex<int>&>(map.find("b")->first);
		auto& c = const_cast<ghl::vertex<int>&>(map.find("c")->first);

		g.add_edge("a", "c", .2f);
		g.add_edge("a", "b", .3f);
		g.add_edge("b", "a", .3f);
		g.add_edge("c", "b", .4f);

		ASSERT_TRUE(g.remove_edge("a", "b"), "expected to successfully remove the edge");

		ASSERT_TRUE(g.has_edge("a", "c"), "expected to not affect other edges")
		ASSERT_TRUE(g.has_edge("b", "a"), "expected to not affect other edges")
		ASSERT_TRUE(g.has_edge("c", "b"), "expected to not affect other edges")
		ASSERT_EQUALS(3, g.num_edges(), "expected to not affect other edges")
		ASSERT_EQUALS(3, g.num_vertices(), "expected to not affect vertices")

		ASSERT_EQUALS(1, a.outdeg, "expected to got deg right")
		ASSERT_EQUALS(1, a.indeg, "expected to got deg right")
		ASSERT_EQUALS(1, b.outdeg, "expected to got deg right")
		ASSERT_EQUALS(1, b.indeg, "expected to got deg right")
		ASSERT_EQUALS(1, c.outdeg, "expected to got deg right")
		ASSERT_EQUALS(1, c.indeg, "expected to got deg right")
	}

ENDDEF_TEST_CASE

DEFINE_TEST_CASE(test_adj_graph_get_directedly_connected_edges)

	// tests for undirected graph
	{
		ghl::adj_list_graph_ds<int> g(true);

		g.add_vertex("a", 1);
		g.add_vertex("b", 2);
		g.add_vertex("c", 3);
		g.add_vertex("d", 4);

		// graph that has no edges connected to the vertex
		{
			ghl::list<typename ghl::adj_list_graph_ds<int>::edge_t> edge_list;
			g.get_adj_in_edges("a", edge_list);

			ASSERT_TRUE(edge_list.empty(), "expected to get no edge")
		}

		// graph that has some edges connected to the vertex
		{
			g.add_edge("a", "b", .1f);
			g.add_edge("c", "a", .2f);
			g.add_edge("a", "d", .3f);

			g.add_edge("b", "c");
			g.add_edge("c", "d");

			ghl::list<typename ghl::adj_list_graph_ds<int>::edge_t> edge_list;
			g.get_adj_in_edges("a", edge_list);

			auto w_contained_in_list = [edge_list](float w) -> bool
			{
				for (auto iter = edge_list.cbegin(); iter != edge_list.cend(); ++iter)
				{
					if (iter->weight == w)
					{
						return true;
					}
				}

				return false;
			};

			ASSERT_EQUALS(3, edge_list.size(), "expected to have exactly 3 edges")
			ASSERT_TRUE(w_contained_in_list(.1f), "expected to have this edge")
			ASSERT_TRUE(w_contained_in_list(.2f), "expected to have this edge")
			ASSERT_TRUE(w_contained_in_list(.3f), "expected to have this edge")
		}
	}
	
	// tests for directed graph
	{
		ghl::adj_list_graph_ds<int> g(false);

		g.add_vertex("a", 1);
		g.add_vertex("b", 2);
		g.add_vertex("c", 3);
		g.add_vertex("d", 4);

		// graph that has no edges connected to the vertex
		{
			ghl::list<typename ghl::adj_list_graph_ds<int>::edge_t> edge_list;
			g.get_adj_in_edges("a", edge_list);

			ASSERT_TRUE(edge_list.empty(), "expected to get no edge")
		}

		// graph that has some edges connected to the vertex
		{
			g.add_edge("a", "b", .1f);
			g.add_edge("c", "a", .5f);
			g.add_edge("a", "c", .2f);
			g.add_edge("a", "d", .3f);

			g.add_edge("b", "c");
			g.add_edge("c", "d");

			ghl::list<typename ghl::adj_list_graph_ds<int>::edge_t> edge_list;
			g.get_adj_in_edges("a", edge_list);

			auto w_contained_in_list = [edge_list](float w) -> bool
			{
				for (auto iter = edge_list.cbegin(); iter != edge_list.cend(); ++iter)
				{
					if (iter->weight == w)
					{
						return true;
					}
				}

				return false;
			};

			ASSERT_EQUALS(3, edge_list.size(), "expected to have exactly 3 edges")
			ASSERT_TRUE(w_contained_in_list(.1f), "expected to have this edge")
			ASSERT_TRUE(w_contained_in_list(.2f), "expected to have this edge")
			ASSERT_TRUE(w_contained_in_list(.3f), "expected to have this edge")
		}
	}

ENDDEF_TEST_CASE

void test_adj_list_graph_ds()
{
	ghl::test_unit ctor_dtor
	{
		{
			&test_adj_graph_ctor
		}, 
		"tests for ctor and dtors of adj_list_graph_ds"
	};

	ghl::test_unit operations
	{
		{
			&test_adj_graph_add_vertex,
			&test_adj_graph_remove_vertex,
			&test_adj_graph_add_edge,
			&test_adj_graph_remove_edge,
			&test_adj_graph_get_directedly_connected_edges
		},
		"tests for operations of adj_list_graph_ds"
	};

	ctor_dtor.execute();
	std::cout << ctor_dtor.get_msg() << "\n";
	operations.execute();
	std::cout << operations.get_msg() << "\n";
}