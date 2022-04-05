#pragma once

#include "list.h"
// will replace std::map with my own map in the future (probably after I learn map in the courses)
#include <map>

#include <string> // for char_traits

namespace ghl
{

	/*
	* unique identifier of a vertex in a graph
	* used for identification and comparasion between vertices
	* if id = 0, then it means that the vertex is invalid
	* 
	* Pass by value is prefered
	*/
	struct vertex_id
	{
		constexpr vertex_id() : id(0) {}
		constexpr explicit vertex_id(uint64_t in_id) : id(in_id) {}
		constexpr explicit vertex_id(const char* name) : id(name_to_id(name)) {}

		constexpr vertex_id(const vertex_id& other) : id(other.id) {}
		constexpr vertex_id(vertex_id&& other) noexcept : id(other.id) { other.id = 0; /* Invalidate the moved-from id */  }

		constexpr bool operator==(vertex_id right) const { return id == right.id; }
		constexpr bool operator==(uint64_t right) const { return id == right; }
		constexpr bool operator==(const char* right) const { return id == name_to_id(right); }
		constexpr bool operator!=(vertex_id right) const { return id != right.id; }
		constexpr bool operator!=(uint64_t right) const { return id != right; }
		constexpr bool operator!=(const char* right) const { return id != name_to_id(right); }

		// less might be needed
		constexpr bool operator<(vertex_id right) const { return id < right.id; }
		constexpr bool operator<(uint64_t right) const { return id < right; }
		constexpr bool operator<(const char* right) const { return id < name_to_id(right); }

		constexpr bool operator<=(vertex_id right) const { return id <= right.id; }
		constexpr bool operator<=(uint64_t right) const { return id <= right; }
		constexpr bool operator<=(const char* right) const { return id <= name_to_id(right); }

		vertex_id operator=(const vertex_id& right) { id = right.id; }
		vertex_id operator=(vertex_id&& right) noexcept { id = right.id; right.id = 0; /* Invalidate the moved-from id */ }
		vertex_id operator=(uint64_t new_id) { id = new_id; }
		vertex_id operator=(const char* new_name) { id = name_to_id(new_name); }

		uint64_t id;

		/*
		* Generates an unique id for a given name.
		* It uses a bijection between name and id, so no collision will occur
		* 
		* If the name is a constexpr, then the result is also a constexpr. Therefore, there is no runtime overhead.
		* 
		* Note: Expected to have no char of the name has a value more than 255. If name contains more than 8 characters (excluding the terminating character), 
		* then only the first 8 characters are used to form an id
		*/
		inline static constexpr uint64_t name_to_id(const char* name)
		{
			uint64_t res = 0;

			auto name_length = std::char_traits<char>::length(name);
			auto min_length = 8 <= name_length ? 8 : name_length;

			for (int i = 0; i < min_length; ++i) // for each char, copy its numerical value to fill one of the 8 bytes of res.
			{
				res |= (uint64_t(*(name + i)) << i * 8);
			}

			return res;
		}
	};

	constexpr bool operator<(uint64_t l, vertex_id r) { return l < r.id; }
	constexpr bool operator<(const char* l, vertex_id r) { return vertex_id::name_to_id(l) < r.id; }

	constexpr bool operator<=(uint64_t l, vertex_id r) { return l <= r.id; }
	constexpr bool operator<=(const char* l, vertex_id r) { return vertex_id::name_to_id(l) <= r.id; }

	// forward declaration
	template <typename T>
	struct vertex;

	/*
	* provided for users to observe the status of a vertex without affecting it
	* can only be observed when the vertex is valid
	* should not be used to modify the degree of a vertex
	*
	* Classes should expose this instead of vertex to users.
	*/
	template <typename T>
	struct vertex_weak_ref
	{
		vertex_weak_ref() {}
		explicit vertex_weak_ref(const vertex<T>& in_v) : pv(&in_v) {}
		vertex_weak_ref(const vertex_weak_ref& other) : pv(other.pv) {}
		vertex_weak_ref(vertex_weak_ref&& other) : pv(other.pv) {}

		bool valid() const { return nullptr != pv; }

		bool operator==(vertex_weak_ref right) const;

		/*
		* ID should be comparable with vertex<T>
		*/
		template <typename ID>
		bool operator==(ID id) const;

		const vertex<T>& observe() const { return *pv; }

		const vertex<T>* pv = nullptr;
	};

	/*
	* The struct that all graph DS defined in gsl should use to represent a vertex in a graph.
	* 
	* Contains info for both directed and undirected graph, which an implementation of graph must (at least choose one to) maintain
	* 
	* Classes should not expose this unless it is necessary. Expose vertex_weak_ref for ordinary use
	*/
	template <typename T>
	struct vertex
	{
		vertex() = delete;
		// note that this definition also covers copy and move construction of T, should T supports them
		template <typename ID, typename... P>
		explicit vertex(ID in_id, P&&... args) : id(in_id), obj(std::make_unique<T>(std::forward<P>(args)...)) {}
		// the following takes the ownership of p
		template <typename ID>
		vertex(ID in_id, T* p) : id(in_id), obj(p) {}

		vertex(const vertex& other) : obj(new T(*other.obj)), id(other.id)
		{
			deg = other.deg;
			indeg = other.indeg;
			outdeg = other.outdeg;
		}
		vertex(vertex&& other) noexcept : id(std::move(other.id)), obj(std::move(other.obj)) 
		{
			deg = other.deg;
			indeg = other.indeg;
			outdeg = other.outdeg;
		}
		vertex& operator=(vertex&& right)
		{
			id = std::move(right.id);
			obj = std::move(right.obj);

			deg = right.deg;
			indeg = right.indeg;
			outdeg = right.outdeg;

			return *this;
		}

		// equality is based on id
		bool operator==(const vertex& right) const { return id == right.id; }
		bool operator!=(const vertex& right) const { return id != right.id; }

		bool operator==(vertex_weak_ref<T> right) const { return right.valid() ? *this == right.observe() : false; }
		bool operator!=(vertex_weak_ref<T> right) const { return right.valid() ? *this != right.observe() : true; }
		// ID is one of : uint64_t, const char*, vertex_id
		template <typename ID>
		bool operator==(ID right) const { return id == right; }
		template <typename ID>
		bool operator!=(ID right) const { return id != right; }

		// @returns true iff it is not an endpoint of any edge.
		bool is_isolated() const { return 0 == deg; }
		// @returns true iff it is valid (a valid id has been given to it)
		bool is_valid() const { return 0 != id; }

		// defined as const so an observer can modify the obj from even a const vertex
		// especially doing so for vertex_weak_ref
		T& get_obj() const { return *obj; }

		// define as an unique ptr to enable moving vertrices even if T doesn't support moving.
		std::unique_ptr<T> obj;

		// the following three degs are defined as mutable so that they can be modified from const vertices.

		// indegree of the vertex (directed)
		// maintained by different DS individually
		mutable size_t indeg = 0;
		// outgree of the vertex (directed)
		// maintained by different DS individually
		mutable size_t outdeg = 0;
		// degree of the vertex (undirected)
		// maintained by different DS individually
		mutable size_t deg = 0;

		// unique identifier of the vertex
		vertex_id id = 0;
	};

	// less might be needed
	template <typename T>
	bool operator<(const vertex<T>& l, const vertex<T>& r)  { return l.id < r.id; }
	template <typename T, typename ID>
	bool operator<(const vertex<T>& l, ID r) { return l.id < r; }
	template <typename T, typename ID>
	bool operator<(ID l, const vertex<T>& r) { return l < r.id; }

	template <typename T>
	bool operator<=(const vertex<T>& l, const vertex<T>& r) { return l.id <= r.id; }
	template <typename T, typename ID>
	bool operator<=(const vertex<T>& l, ID r) { return l.id <= r; }
	template <typename T, typename ID>
	bool operator<=(ID l, const vertex<T>& r) { return l <= r.id; }

	template<typename T>
	inline bool ghl::vertex_weak_ref<T>::operator==(vertex_weak_ref right) const
	{
		return *pv == *(right.pv);
	}

	template<typename T> 
	template<typename ID>
	inline bool ghl::vertex_weak_ref<T>::operator==(ID id) const
	{
		return *pv == id;
	}

	/*
	* When the user asks for an edge, all implementations should give an edge of an instantiation of this struct
	* 
	* W is the type of the weight of the edge, which should act like a number
	* 
	* In many cases, one of the left, right vertices is not needed, as an edge is often obtained by giving a vertex.
	* However, we cannot say so for all cases, so we have to include all info in this struct, which is the cost of a abstraction of so high a level.
	* (I guess only LORD God can make perfect things that have no cost at all)
	*/
	template <typename T, typename W>
	struct edge
	{
		edge() {}
		edge(const vertex<T>& in_left, const vertex<T>& in_right, const W& wt) :
			left(in_left), right(in_right), weight(wt) {}
		edge(vertex_weak_ref<T> in_left, vertex_weak_ref<T> in_right, const W& wt) :
			left(in_left), right(in_right), weight(wt) {}
		edge(const edge& other) : 
			left(other.left), right(other.right), weight(other.weight) {}

		~edge() {}

		edge& operator=(const edge& right)
		{
			left = right.left; this->right = right.right; weight = right.weight;
			return *this;
		}

		bool operator<=(const edge& right) const { return weight <= right.weight; }

		bool valid() const { return left.valid() && right.valid(); }

		template <typename V>
		bool contains(V v) const { return left == v || right == v; }

		vertex_weak_ref<T> left, right;
		W weight;
	};

	template <typename T>
	using float_weighted_edge = edge<T, float>;

	/*
	* ADT graph:
	* Represents mathematically any graph.
	* It can be regarded as an interface that provides basic methods all graphs should support 
	* It does not care about how a DS implements a graph, but only cares about the common methods that a DS provides 
	* (c.f. the template class ghl::tree)
	* 
	* Requirements:
	* T is the type of the actual data structure that implements a graph, which must satisfy:
	* 1. types
	*		T::obj_t is the type of the object stored in each vertex
	*		T::weak_ref_t is vertex_weak_ref<T::obj_t>
	*		T::edge_t is edge<T::obj_t, W>, where W is a known type between the user and the writer of T, which is used for the weight of edges
	*		T::iterator is a kind of iterator that allows the user to iterate through all vertices directedly connected to a vertex, as mentioned below.
	* 2. T should store its vertices via template struct vertex, maintain the info in the struct, 
	*		return info about vertices in vertex_weak_ref, return info about edges in edge, as mentioned above in the comments of these structs.
	* 3. operations
	*		a. given ID and other arguments for constructing a vertex, T provides add_vertex() which takes these arguments and constructs a vertex of ID in the map 
	*	(if no current vertrices exist with ID). Its return value should be a weak ref to the vertex or to the vertex that blocked the creation
	*		b. given ID, T provides find_vertex() that finds a vertex of ID and returns a weak ref to it
	*		c. given ID, T provides remove_vertex() that removes a vertex of ID and returns a boolean that indicates if such a vertex was found and removed.
	*		d. given the IDs of two vertices, T provides add_edge() that adds an edge of the two vertices (if found), and returns a boolean that indicates if such an edge was added.
	*		e. given the IDs of two vertices, T provides has_edge() that returns a boolean that indicates if an edge of the two vertices is present
	*	and get_edge that returns a edge_t representing the edge. 
	*	note that, if T contains a multigraph, and multiple edges of the same two IDs are present, then it is not specified here which one of them is to be returned.
	*		f. given the IDs of two vertices, T provides removes_edge() that removes an edge of the two vertices (if found), and returns a boolean that indicates if such an edge was removed
	*	note that, if T contains a multigraph, and multiple edges of the same two IDs are present, then it is not specified here which one of them is to be removed.
	*		g. given the ID of a vertex, T provides get_directly_connected_edges() that returns an iterator (to T::edge_t) that iterates throughs all edges that are directedly connected to the vertex
	*	The iterator must support operator*(), has_next(), next(), is_valid(), and operator++ (prefix and postfix), 
	*	where next() and ++ forwards the iterator, has_next() indicates if it can still be forwarded, is_valid() indicates if it refers to an valid edge, and operator* gets the edge_t it refers to.
	*		h. get_all_vertices which takes a reference to a list of weak_ref_t and stored references to all vertices in it
	*		i. get_all_edges which takes a reference to a list of edge_t and stored references to all edges in it (for undirected maps, all edges are stored twice, with two possible orders of the endpoints)
	*		j. num_vertices() and num_edges()
	*		k. empty()
	*		l. is_undirected()
	* 4. construction
	*		T has a default constructor that gives an empty graph
	*		T has an explicit constructor that takes a boolean to decide if it will be undirected (true) or directed (false)
	*		
	* 
	* Rep Invariant:
	* Depends on the DS's invariant
	*/
	template <typename T>
	class graph final
	{
	public:
		using obj_t = typename T::obj_t;
		using weak_ref_t = typename T::weak_ref_t;
		using edge_t = typename T::edge_t;
		using edge_iter = typename T::iterator;

	public:
		graph() : imp(std::make_unique<T>()) {}
		graph(bool undirected) : imp(std::make_unique<T>(undirected)) {}
		template <typename... P>
		graph(P&& ... args) : imp(std::forward<P>(args)...) {}

		graph(const graph&) = delete;
		graph(graph&& other) : imp(std::move(other.imp)) {}
		graph& operator=(graph& r) { imp = std::move(r.imp); return *this; }

		~graph() {}

	public:
		inline bool empty() const { return imp->empty(); }
		inline bool is_undirected() const { return imp->is_undirected(); }

		inline size_t num_vertices() const { return imp->num_vertices(); }
		inline size_t num_edges() const { imp->num_edges(); }

		template <typename ID, typename... P>
		inline weak_ref_t add_vertex(ID id, P&& ... args) { return imp->add_vertex(id, std::forward<P>(args)...); }
		template <typename ID>
		inline bool remove_vertex(ID id) { return imp->remove_vertex(id); }
		template <typename ID>
		inline weak_ref_t find_vertex(ID id) const { return imp->find_vertex(id); }

		template <typename ID, typename W>
		inline bool add_edge(ID left, ID right, W weight) { return imp->add_edge(left, right, weight); }
		template <typename ID>
		inline bool has_edge(ID left, ID right) const { return imp->has_edge(left, right); }
		template <typename ID>
		inline edge_t get_edge(ID left, ID right) const { return imp->get_edge(left, right); }
		template <typename ID>
		inline bool get_edge(ID left, ID right) { return imp->remove_edge(left, right); }

		template <typename ID>
		inline edge_iter get_directly_connected_edges(ID id) const { return imp->get_directly_connected_edges(id); }

	private:
		// the actual implementation
		std::unique_ptr<T> imp;
	};

	/*
	* Implementation of a graph using adjacency lists
	* 
	* Can be used as either directed and undirected.
	* If used as directed, then the adj lists contains all vertices a vertex leads to.
	* If used as undirected, then the two endpoint vertices both have a ref to each other in adj lists.
	* 
	* Supports adding weight to edges. If not set, all weights are by default 0.0f
	*/
	template <typename T>
	class adj_list_graph_ds
	{
		// for unit tests
		friend class adj_list_graph_ds_tester;

	public:
		using obj_t = T;
		using vertex_t = vertex<T>;
		using weak_ref_t = vertex_weak_ref<T>;
		using edge_t = float_weighted_edge<T>;

		/*
		* Used in adj lists to reference vertices
		*
		* We can store edges directly, but which is 1/2 more expensive than using this that contains just enough info
		*/
		struct vertex_ref
		{
			vertex_ref() {}
			vertex_ref(const vertex_t& in_v, float wt = 0.0f) : v(&in_v), weight(wt) {}
			vertex_ref(const vertex_ref& other) : v(other.v), weight(other.weight) {}

			~vertex_ref() {}

			vertex_ref& operator=(const vertex_ref& right)
			{
				v = right.v; weight = right.weight;
				return *this;
			}

			bool valid() const { return v != nullptr; }

			// 0.0f if it either the graph is unweighted or the edge is connecting the same vertex
			float weight = 0.0f;

			const vertex_t* v;
		};

	private:
		using pair_t = std::pair<vertex_t, ghl::list<vertex_ref>>;

	public:
		adj_list_graph_ds() {}
		explicit adj_list_graph_ds(bool b_undirected) : undirected(b_undirected) {}

		~adj_list_graph_ds() {}

	public:
		bool empty() const { return vertices_and_lists.empty(); }
		bool is_undirected() const { return undirected; }

		size_t num_vertices() const { return vertices_and_lists.size(); }
		size_t num_edges() const
		{
			size_t res = 0;
			for (const auto& p : vertices_and_lists)
			{
				res += p.second.size();
			}

			// all edges are counted twice for undirected graph
			return undirected ? res / 2 : res;
		}

	public:
		/*
		* Adds a vertex to the map. If there is already such a vertex, do nothing
		* 
		* Regardless of what kind of vertex we want to add,
		* ID must be one of uint64_t, const char*, or vertex_id, which serves as the unique id of the vertex to be added
		* 
		* @returns a weak ref to the vertex added, or a weak ref to the already existing vertex
		*/
		template <typename ID, typename... P>
		weak_ref_t add_vertex(ID id, P&& ... args)
		{
			auto iter = vertices_and_lists.emplace(vertex_t(id, std::forward<P>(args)...), ghl::list<vertex_ref>{}).first;
			return weak_ref_t(iter->first);
		}

		/*
		* Removes the vertex of id, and all edges connected to it
		*/
		bool remove_vertex(uint64_t id)
		{
			auto i = vertices_and_lists.find(id);

			if (i != vertices_and_lists.end()) // found the vertex
			{
				// first, for each adj list, remove the ref to the vertex from it.
				for (auto& pair : vertices_and_lists)
				{
					auto& adj_list = pair.second;
					for (auto iter = adj_list.begin(); iter != adj_list.end(); ++iter)
					{
						if (*(iter->v) == id)
						{
							// adjust the deg
							if (undirected)
							{
								--(pair.first.deg);
							}
							else
							{
								--(pair.first.outdeg);
							}

							iter = adj_list.remove(iter); // remove the vertex ref and update iter
						}
					}

					// after all refs are gone, remove the vertex and its adj_list
					vertices_and_lists.erase(i);

					return true;
				}
				
				return false;
			}
		}
		bool remove_vertex(const char* name) { return remove_vertex(vertex_id::name_to_id(name)); }
		bool remove_vertex(const weak_ref_t& v) { return v.valid() ? remove_vertex(v.observe().id) : false; }

		/*
		* @returns a weak ref to the vertex that has id if found, or an invalid ref otherwise.
		*/
		weak_ref_t find_vertex(uint64_t id) const
		{
			auto iter = vertices_and_lists.find(id);
			if (vertices_and_lists.end() != iter) // found;
			{
				return weak_ref_t(iter->first);
			}
			else
			{
				return weak_ref_t(); // return an invalid one
			}
		}
		/*
		* @returns a weak ref to the vertex that has name if found, or an invalid ref otherwise.
		*/
		weak_ref_t find_vertex(const char* name) const { return find_vertex(vertex_id::name_to_id(name)); }

		/*
		* If left and right are present (V must be (possibly cv-qualified) either weak_ref_t, uint64_t, const char*, or vertex_id)
		* then add an unordered edge containing left and right if the graph is undirected or adds an edge from left to right if directed
		* Otherwise, do nothing.
		* 
		* @returns true iff the edge is added
		*/
		template <typename V>
		bool add_edge(V left, V right, float weight = 0.0f)
		{
			auto li = vertices_and_lists.find(left), ri = vertices_and_lists.find(right);
			if (li != vertices_and_lists.end() && ri != vertices_and_lists.end()) // vertices are present
			{
				if (undirected) // add the edge to the list of both left and right
				{
					++(li->first.deg); ++(ri->first.deg);
					li->second.emplace_back(ri->first, weight);
					ri->second.emplace_back(li->first, weight);
				}
				else // add the edge to only the list of left
				{
					++(li->first.outdeg); ++(ri->first.indeg);
					li->second.emplace_back(ri->first, weight);
				}

				return true;
			}

			return false;
		}

		/*
		* @returns true iff the graph has an edge of left and right
		*/
		template <typename V>
		bool has_edge(V left, V right)
		{
			// right should appear in left's adj list for regardless of the graph being directed or undirected.
			auto i = vertices_and_lists.find(left);
			if (i != vertices_and_lists.end())
			{
				for (const auto& edge : i->second)
				{
					if (*(edge.v) == right) // edges in adj lists can never be invalid
					{
						return true;
					}
				}
			}
			return false;
		}

		/*
		* Find the weight of the edge determined by left and right.
		* 
		* @returns the edge if found. Otherwise an invalid edge.
		*/
		template <typename V>
		edge_t get_edge(V left, V right)
		{
			auto i = vertices_and_lists.find(left);
			if (i != vertices_and_lists.end())
			{
				for (const auto& edge : i->second)
				{
					if (*(edge.v) == right) // edges in adj lists can never be invalid
					{
						return float_weighted_edge<T>(i->first, *(edge.v), edge.weight);
					}
				}
			}

			return float_weighted_edge<T>();
		}

		/*
		* Removes the edge that is (left, right) or {left, right} (depending on directed or not)
		* 
		* @returns true iff the edge is successfully removed
		*/
		template <typename V>
		bool remove_edge(V left, V right)
		{
			auto il = vertices_and_lists.find(left), ir = vertices_and_lists.find(right);
			if (il != vertices_and_lists.end() && ir != vertices_and_lists.end()) // found both vertices
			{
				// first, find if the edge exists

				auto& l_adj_list = il->second;
				// right should appear in left's adj list for regardless of the graph being directed or undirected.
				bool edge_found = false;
				auto liter = l_adj_list.begin();
				for (; liter != l_adj_list.end(); ++liter)
				{
					if (*(liter->v) == right)
					{
						edge_found = true;
						break;
					}
				}

				if (edge_found) // now liter should point to the right vertex
				{
					if (undirected) // need to remove the edge from both left's and right's list
					{
						// update deg
						--(il->first.deg); --(ir->first.deg);

						// remove right from left's list
						l_adj_list.remove(liter);

						// remove left from right's list
						auto& r_adj_list = ir->second;
						for (auto riter = r_adj_list.begin(); riter != r_adj_list.end(); ++riter)
						{
							if (*(riter->v) == left)
							{
								r_adj_list.remove(riter);
								break;
							}
						}
					}
					else // need to remove the edge from only left's list
					{
						--(il->first.outdeg); --(ir->first.indeg);
						
						// remove right from left's list
						l_adj_list.remove(liter);
					}

					return true;
				}

			}

			return false;
		}

		/*
		* Gives references to all vertices to in_list
		*/
		void get_all_vertices(ghl::list<weak_ref_t>& in_list)
		{
			for (const auto& p : vertices_and_lists)
			{
				in_list.emplace_back(p.first);
			}
		}
		/*
		* Gives references to all edges to in_list
		* 
		* Note that for undirected map, all edges {a,b} are given twice as {a,b} and {b,a}
		*/
		void get_all_edges(ghl::list<edge_t>& in_list)
		{
			for (const auto& p : vertices_and_lists)
			{
				for (const auto& v_ref : p.second)
				{
					in_list.emplace_back(p.first, *(v_ref.v), v_ref.weight);
				}
			}
		}

	public:

		/*
		* V has to be one of: vertex_t, weak_ref_t, or the types that can result in a vertex id
		* 
		* Fills all edges directly connected to V to list
		*/
		template <typename V>
		void get_directly_connected_edges(V v, ghl::list<edge_t> & list) const
		{
			auto i = vertices_and_lists.find(v);

			if (i != vertices_and_lists.end())
			{
				const auto& adj_list = i->second;
				const auto& v = i->first;
				for (const auto& v_ref : adj_list)
				{
					list.emplace_back(v, *(v_ref.v), v_ref.weight);
				}
			}
		}

	private:
		// true = undirected, false = directed
		bool undirected = true;

		std::map<vertex_t, ghl::list<vertex_ref>, std::less<>> vertices_and_lists;
	};
}