#pragma once

#include "list.h"

namespace ghl
{
	/*
	* Trees will be represented by the tree interface.
	* The implementations of different trees are hidden under each different node class.
	*/

	enum class tree_traversal_type : char
	{
		preorder,
		inorder,
		postorder,
		breadth_first
	};

	/*
	* Listener of a traversal
	* T is the type of the object contained in nodes
	*/
	template <typename T>
	class tree_traversal_listener
	{
		tree_traversal_listener() {}
		virtual ~tree_traversal_listener() {}

		void enter_node(T& obj) = 0;
		void exit_node(T& obj) = 0;
	};

	/*
	* ADT tree: represents any tree
	* 
	* This class is but an encapsulation of the underlying implementation of a tree. It provides basic methods that all tree should support.
	* This class may be extended to provide special methods for special trees.
	* 
	* Advantages for this encapsulation:
	* 1. the implementation detail can be hidden (as long as the node is not a template class)
	* 2. the size of any tree object is fixed
	* 3. different implementations do not necessarily need to use inheritance to have the same exposed methods; completely zero-overhead.
	* 4. easy for users
	* Disadvantages:
	* 1. hard for maintainers
	* 2. require runtime memory allocation (an ordinary tree would need this anyway)
	* 
	* T is the type of a specific kind of node, which must support:
	* 1. getting the number of branches of the node
	*		via num_branches() method
	* 2. iterations through all branches
	*		via iterators defined in node
	* 3. iterators return pointer to node (std::unique_ptr is also acceptable) when dereferenced. This is because some branch may be empty on a tree. 
			If a branch is empty, the iterator to it should return nullptr when dereferencing.
	* 4. getting the element contained it the node
	*		type of the obj must be defined as obj_t
	*		obj is obtained via get_obj() method
	* 5. satisfies the requirments of std::unique_ptr
	* 
	* The tree uniquely owns the node. Users give the class an inited pointer, and it will take the ownership of the node.
	*/
	template <typename T>
	class tree
	{
	public:
		using node_t = T;
		using obj_t = typename T::obj_t;

	public:
		// takes ownership of ptr
		tree(node_t* ptr) : root(ptr) {}
		// other relinquishes the ownership to this.
		tree(tree&& other) : root(std::move(other.root)) {}
		// intend to allow inheritance
		virtual ~tree() {}

		tree& operator=(tree&& right)
		{
			root = std::move(right.root);
			return *this;
		}

	protected:
		std::unique_ptr<node_t> root;

		node_t& get_root() { return *root; }
		const node_t& get_root() const { return *root; }

	public:
		// @returns true iff the tree is empty
		bool empty() const { return root->empty(); }

		// resets the object (destructs it and sets the pointer to nullptr)
		void reset_object() { root->reset_object(); }

		/*
		* Traverse the tree on given type with listener
		* 
		* for now, only define traversal for preorder, postorder, and breadth-first
		* inorder traversal will be defined in the binary tree specialization
		*/
		inline void traverse(tree_traversal_listener<obj_t>& listener, tree_traversal_type type)
		{
			if (tree_traversal_type::preorder == type)
			{
				traverse_node_preorder(listener, *root);
			}
			else if (tree_traversal_type::postorder == type)
			{
				traverse_node_postorder(listener, *root);
			}
			else if (tree_traversal_type::breadth_first == type)
			{
				traverse_node_breadth_first(listener, *root);
			}
		}

	private:
		/*
		* Recursively traverses the node and its branches in preorder
		*/
		static void traverse_node_preorder(tree_traversal_listener<obj_t>& listener, node_t& node)
		{
			listener.enter_node(node.get_obj());
			listener.exit_node(node.get_obj());

			for (const auto& b : node)
			{
				traverse_node_preorder(listener, b);
			}
		}
		/*
		* Recursively traverses the node and its branches in postorder
		*/
		static void traverse_node_postorder(tree_traversal_listener<obj_t>& listener, node_t& node)
		{
			for (const auto& b : node)
			{
				traverse_node_postorder(listener, b);
			}

			listener.enter_node(node.get_obj());
			listener.exit_node(node.get_obj());
		}
		/*
		* Traverses the node and its branches breadth-firstly.
		* 
		* This cannot be done recursively, as a node is not aware of nodes in the same level as it is.
		*/
		static void traverse_node_breadth_first(tree_traversal_listener<obj_t>& listener, node_t& node)
		{
			// traverse the queue from front to back
			ghl::list<const node_t*> traversal_queue{ &node };
			
			/*
			* Traverse front, putting all its direct branches at back, and remove front
			*/
			while (!traversal_queue.empty())
			{
				const auto* n = traversal_queue.front();

				listener.enter_node(n->get_obj());
				listener.exit_node(n->get_obj());

				for (const auto& b : *n)
				{
					traversal_queue.insert_back(&b);
				}

				traversal_queue.remove_front();
			}
		}
	};

	/*
	* node (subtree) that has fixed number of branches
	* 
	* T must be constructiable and destructiable
	*/
	template <typename T, size_t N>
	class fixed_size_node
	{
		friend class tree<fixed_size_node>;

	public:
		using obj_t = T;

	public:
		/*
		* Init obj and take ownership of in_branches
		*/
		explicit fixed_size_node(fixed_size_node* parent = nullptr, T* obj = nullptr, fixed_size_node* in_branches[N] = {}) :
			p_obj(obj),
			p(parent)
		{
			for (size_t i = 0; i != N; ++i)
			{
				if (in_branches[i] != nullptr)
				{
					++num_non_empty_branches;
				}

				branches[i].reset(in_branches[i]);
			}
		}
		// we cannot assume we can copy the obj
		fixed_size_node(const fixed_size_node& other) = delete;
		fixed_size_node(fixed_size_node&& other) 
			: 
			p_obj(std::move(other.p_obj)), p(other.p),
			num_non_empty_branches(other.num_non_empty_branches)
		{
			for (size_t i = 0; i != N; ++i)
			{
				branches[i] = std::move(other.branches[i]);
			}
		}
		fixed_size_node& operator=(fixed_size_node&& right)
		{
			p_obj = std::move(right.p_obj);
			p = right.p;
			num_non_empty_branches = right.num_non_empty_branches;

			for (size_t i = 0; i != N; ++i)
			{
				branches[i] = std::move(right.branches[i]);
			}

			return *this;
		}

		~fixed_size_node() {}

	public:
		static constexpr size_t num_branches() { return N; }

		T& get_obj() { return *p_obj; }
		const T& get_obj() const { return *p_obj; }

		/*
		* @returns true iff the branch at ind is empty

		*/
		bool branch_empty(size_t ind) const { return nullptr == branches[ind]; }

		/*
		* @returns true iff the tree is empty
		* (i.e. all branches are empty and the node contains no element)
		*/
		bool empty() const { return 0 == num_non_empty_branches && nullptr == p_obj; }

		/*
		* Sets the branch at ind iff it is empty
		*/
		inline void set_branch(size_t ind, fixed_size_node* branch)
		{
			if (nullptr == branches[ind])
			{
				++num_non_empty_branches;
				branches[ind] = std::unique_ptr<fixed_size_node>(branch);
			}
		}
		/*
		* Resets the branch at ind to empty and consequently destructs the branch
		*/
		void reset_branch(size_t ind)
		{
			if (nullptr != branches[ind])
			{
				branches[ind].reset(nullptr);
				--num_non_empty_branches;
			}
		}

	public:
		using iterator = std::unique_ptr<fixed_size_node>*;
		using const_iterator = const std::unique_ptr<fixed_size_node>*;

		iterator begin() { return &branches[0]; }
		iterator end() { return &branches[0] + N; }
		const_iterator begin() const { return &branches[0]; }
		const_iterator end() const { return &branches[0] + N; }
		const_iterator cbegin() const { return &branches[0]; }
		const_iterator cend() const { return &branches[0] + N; }

		void reset_object() { p_obj->reset(nullptr); }

	public:
		std::unique_ptr<fixed_size_node> branches[N];
		// store it as pointer so that nodes can be easily moved
		std::unique_ptr<T> p_obj;

		// number of NON-EMPTY branches
		size_t num_non_empty_branches;

	public:
		// its direct parent
		fixed_size_node* p = nullptr;
	};

	template <typename T>
	using binary_node = fixed_size_node<T, 2>;
	template <typename T>
	using binary_tree = tree<binary_node<T>>;

	// partial specialization for binary node
	template <typename T>
	class fixed_size_node<T, 2>
	{
		friend class tree<fixed_size_node>;

	public:
		using obj_t = T;

	public:
		/*
		* Init obj and take ownership of in_branches
		*/
		template <typename... P>
		fixed_size_node(fixed_size_node* parent = nullptr, T* obj = nullptr, fixed_size_node* in_left = nullptr, fixed_size_node* in_right = nullptr) :
			p_obj(obj),
			p(parent)
		{
			branches[0].reset(in_left); branches[1].reset(in_right);
		}
		// we cannot assume we can copy the obj
		fixed_size_node(const fixed_size_node& other) = delete;
		fixed_size_node(fixed_size_node&& other) : p_obj(std::move(other.p_obj)), p(other.p)
		{
			for (size_t i = 0; i != 2; ++i)
			{
				branches[i] = std::move(other.branches[i]);
			}
		}
		fixed_size_node& operator=(fixed_size_node&& right)
		{
			p_obj = std::move(right.p_obj);
			p = right.p;

			for (size_t i = 0; i != 2; ++i)
			{
				branches[i] = std::move(right.branches[i]);
			}

			return *this;
		}

	public:
		static constexpr size_t num_branches() { return 2; }

		T& get_obj() { return *p_obj; }
		const T& get_obj() const { return *p_obj; }

		/*
		* @returns true iff the branch at ind is empty
		*/
		bool branch_empty(size_t ind) const { return nullptr == branches[ind]; }

		/*
		* @returns true iff the tree is empty
		* (i.e. all branches are empty and the node contains no element)
		*/
		inline bool empty() const { return nullptr == left() && nullptr == right() && nullptr == p_obj; }

	public:
		fixed_size_node* left() { return branches[0].get(); }
		fixed_size_node* right() { return branches[1].get(); }
		const fixed_size_node* left() const { return branches[0].get(); }
		const fixed_size_node* right() const { return branches[1].get(); }

		void set_left(fixed_size_node* n) { branches[0].reset(n); }
		void set_right(fixed_size_node* n) { branches[1].reset(n); }
		void reset_left() { branches[0].reset(); }
		void reset_right() { branches[1].reset(); }
		
		void reset_object() { p_obj.reset(); }

	public:
		using iterator = std::unique_ptr<fixed_size_node>*;
		using const_iterator = const std::unique_ptr<fixed_size_node>*;

		iterator begin() { return &branches[0]; }
		iterator end() { return &branches[0] + 2; }
		const_iterator begin() const { return &branches[0]; }
		const_iterator end() const { return &branches[0] + 2; }
		const_iterator cbegin() const { return &branches[0]; }
		const_iterator cend() const { return &branches[0] + 2; }

	public:
		// still declare it as an array so that left and right are continuous in memory
		std::unique_ptr<fixed_size_node> branches[2];
		// store it as pointer so that nodes can be easily copied and constructed
		std::unique_ptr<T> p_obj;

	public:
		// its direct parent
		fixed_size_node* p = nullptr;
	};

	// partial specilization for binary tree
	// T must additionally support left() and right() which returns the left and right branch, respectively.
	template <typename T>
	class tree<binary_node<T>>
	{
	public:
		using node_t = binary_node<T>;
		using obj_t = typename T;

	public:
		tree() : root(nullptr) {};

	protected:
		std::unique_ptr<node_t> root;

	public:
		// takes ownership of ptr
		tree(node_t* ptr) : root(ptr) {}
		// other relinquishes the ownership to this.
		tree(tree&& other) : root(std::move(other.root)) {}
		// intend to allow inheritance
		~tree() {}

	public:
		node_t* get_root() { return root.get(); }
		const node_t* get_root() const { return root.get(); }

		// @returns true iff the tree is empty
		bool empty() const { return root != nullptr ? root->empty() : true; }

		bool left_empty() const { return root->branch_empty(0); }
		bool right_empty() const { return root->branch_empty(1); }

		node_t* left() { return root->left(); }
		node_t* right() { return root->right(); }
		const node_t* left() const { return root->left(); }
		const node_t* right() const { return root->right(); }

		// the first two takes the pointer to nd and takes its ownership
		// the second two construct a new node with obj

		void set_left(node_t* nd) { root->set_left(nd); }
		void set_right(node_t* nd) { root->set_right(nd); }
		void set_left(T* obj) { root->set_left(new node_t(root.get(), obj)); }
		void set_right(T* obj) { root->set_right(new node_t(root.get(), obj)); }

		void reset_left() { root->reset_left(); }
		void reset_right() { root->reset_right(); }

		void reset_object() { root->reset_object(); }

		void destroy() { root.reset(nullptr); }

	public:
		/*
		* Traverse the tree on given type with listener
		*
		* for now, only define traversal for preorder, postorder, and breadth-first
		* inorder traversal will be defined in the binary tree specialization
		*/
		inline void traverse(tree_traversal_listener<obj_t>& listener, tree_traversal_type type)
		{
			if (tree_traversal_type::preorder == type)
			{
				traverse_node_preorder(listener, *root);
			}
			else if (tree_traversal_type::postorder == type)
			{
				traverse_node_postorder(listener, *root);
			}
			else if (tree_traversal_type::breadth_first == type)
			{
				traverse_node_breadth_first(listener, *root);
			}
			else if (tree_traversal_type::inorder == type)
			{
				traverse_node_inorder(listener, *root);
			}
		}

	private:
		/*
		* Recursively traverses the node and its branches in preorder
		*/
		static void traverse_node_preorder(tree_traversal_listener<obj_t>& listener, node_t& node)
		{
			listener.enter_node(node.get_obj());
			listener.exit_node(node.get_obj());

			for (const auto& b : node)
			{
				traverse_node_preorder(listener, b);
			}
		}
		/*
		* Recursively traverses the node and its branches in inorder
		*/
		static void traverse_node_inorder(tree_traversal_listener<obj_t>& listener, node_t& node)
		{
			traverse_node_inorder(listener, node.left());

			listener.enter_node(node.get_obj());
			listener.exit_node(node.get_obj());

			traverse_node_inorder(listener, node.right());
		}
		/*
		* Recursively traverses the node and its branches in postorder
		*/
		static void traverse_node_postorder(tree_traversal_listener<obj_t>& listener, node_t& node)
		{
			for (const auto& b : node)
			{
				traverse_node_postorder(listener, b);
			}

			listener.enter_node(node.get_obj());
			listener.exit_node(node.get_obj());
		}
		/*
		* Traverses the node and its branches breadth-firstly.
		*
		* This cannot be done recursively, as a node is not aware of nodes in the same level as it is.
		*/
		static void traverse_node_breadth_first(tree_traversal_listener<obj_t>& listener, node_t& node)
		{
			// traverse the queue from front to back
			ghl::list<const node_t*> traversal_queue{ &node };

			/*
			* Traverse front, putting all its direct branches at back, and remove front
			*/
			while (!traversal_queue.empty())
			{
				const auto* n = traversal_queue.front();

				listener.enter_node(n->get_obj());
				listener.exit_node(n->get_obj());

				for (const auto& b : *n)
				{
					traversal_queue.insert_back(&b);
				}

				traversal_queue.remove_front();
			}
		}
	};

	// node that can contain arbitrary number of branches
	template <typename T>
	class general_node
	{
		friend class tree<general_node>;

	private:
		using container_t = ghl::list<general_node>;
	public:
		using obj_t = T;

	public:
		size_t num_branches() const { return branches.size(); }

		T& get_obj() { return obj; }
		const T& get_obj() const { return obj; }

	public:
		using iterator = typename container_t::iterator;
		using const_iterator = typename container_t::const_iterator;

		iterator begin() { return branches.begin(); }
		iterator end() { return branches.end(); }
		const_iterator cbegin() const { return branches.cbegin(); }
		const_iterator cend() const { return branches.cend(); }

	private:
		ghl::list<general_node> branches;

		T obj;
	};
}