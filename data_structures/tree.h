#pragma once

#include "list.h"

namespace ghl
{
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
	* nodes that have fixed number of branches
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
				branches[i].reset(in_branches[i]);
			}
		}
		// we cannot assume we can copy the obj
		fixed_size_node(const fixed_size_node& other) = delete;
		fixed_size_node(fixed_size_node&& other) : p_obj(std::move(other.p_obj)), p(other.p)
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

			for (size_t i = 0; i != N; ++i)
			{
				branches[i] = std::move(right.branches[i]);
			}

			return *this;
		}

		~fixed_size_node() {}

	public:
		inline size_t num_branches() const { return N; }

		T& get_obj() { return *p_obj; }
		const T& get_obj() const { return *p_obj; }

		/*
		* @returns true iff the branch at ind is empty
		*/
		bool branch_empty(size_t ind) const { return nullptr == branches[ind]; }

		/*
		* Sets the branch at ind iff it is empty
		*/
		inline void set_branch(size_t ind, fixed_size_node* branch)
		{
			if (nullptr == branches[ind])
			{
				branches[ind] = std::unique_ptr<fixed_size_node>(branch);
			}
		}
		/*
		* Resets the branch at ind to empty and consequently destructs the branch
		*/
		void reset_branch(size_t ind)
		{
			branches[ind].reset(nullptr);
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

	public:
		std::unique_ptr<fixed_size_node> branches[N];
		// store it as pointer so that nodes can be easily moved
		std::unique_ptr<T> p_obj;

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

	private:
		using container_t = std::array<std::unique_ptr<fixed_size_node>, 2>;
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
		inline size_t num_branches() const { return 2; }

		T& get_obj() { return *p_obj; }
		const T& get_obj() const { return *p_obj; }

		/*
		* @returns true iff the branch at ind is empty
		*/
		bool branch_empty(size_t ind) const { return nullptr == branches[ind]; }

		/*
		* Sets the branch at ind iff it is empty (0 for left, 1 for right)
		*/
		inline void set_branch(size_t ind, fixed_size_node* branch)
		{
			if (nullptr == branches[ind])
			{
				branches[ind] = std::unique_ptr<fixed_size_node>(branch);
			}
		}
		/*
		* Resets the branch at ind to empty and consequently destructs it (0 for left, 1 for right)
		*/
		void reset_branch(size_t ind)
		{
			branches[ind].reset(nullptr);
		}

	public:
		fixed_size_node* left() { return branches[0].get(); }
		fixed_size_node* right() { return branches[1].get(); }
		const fixed_size_node* left() const { return branches[0].get(); }
		const fixed_size_node* right() const { return branches[1].get(); }

		void set_left(fixed_size_node* n) { branches[0].reset(n); }
		void set_right(fixed_size_node* n) { branches[1].reset(n); }

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
	class tree<fixed_size_node<T, 2>>
	{
	public:
		using node_t = fixed_size_node<T, 2>;
		using obj_t = typename T;

	public:
		tree() {};

	protected:
		std::unique_ptr<node_t> root;

		node_t& get_root() { return *root; }
		const node_t& get_root() const { return *root; }

	public:
		// takes ownership of ptr
		tree(node_t* ptr) : root(ptr) {}
		// other relinquishes the ownership to this.
		tree(tree&& other) : root(std::move(other.root)) {}
		// intend to allow inheritance
		~tree() {}

	public:
		node_t* left() { return root->left(); }
		node_t* right() { return root->right(); }
		const node_t* left() const { return root->left(); }
		const node_t* right() const { return root->right(); }

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

	/*
	* T must have a total ordering imposed by operator<=, and equality imposed by operator==
	* If T can be represented by light weight key, then T must also have operator<= and operator== that take a key
	* 
	* Invariant: the binary search tree property.
	*/
	template <typename T>
	class binary_search_tree : binary_tree<T>
	{
	public:
		binary_search_tree() {}
		/*
		* Assumes that the pointers own the elements
		* After it, the ownerships will be taken
		*/
		binary_search_tree(const std::initializer_list<T*>& init_list)
		{
			for (auto *p: init_list)
			{
				insert(p);
			}
		}

	public:
		friend class iterator;

		struct iterator
		{
			iterator() {}
			explicit iterator(binary_node<T>* n) : node(n) {}
			iterator(const iterator& o) : node(o.node) {}
			iterator(iterator&& o) : node(o.node) {}
			~iterator() {}

			iterator& operator=(const iterator& r) { node = r.node; return *this; }
			iterator& operator=(iterator&& r) { node = r.node; return *this; }

			bool valid() const { return nullptr != node; }

			iterator successor() const
			{
				if (node->right() != nullptr)
				{
					return iterator(binary_search_tree::internal_minimum(node->right()));
				}
				else
				{
					// if its children have no value bigger than the val it has,
					// then a bigger value can only be held by some of its parent who's bigger than the branch contains it
					// (i.e. it or its some parent is a left child of a node)

					binary_node<T>* child, parent = node;
					do
					{
						child = parent;
						parent = parent->p;
					} while (parent != nullptr && child == parent->right()); // go back to its parent until the parent is nullptr or until we have a left child

					// if parent is not nullptr, it is the first parent that has a left child in the branch
					// by the property, it is the smallest one bigger than this
					// or otherwise, there are no bigger val in the tree than this, and we return parent = nullptr;
					return iterator(parent);
				}
			}
			iterator predecessor() const
			{
				if (node->left() != nullptr)
				{
					return binary_search_tree::internal_maximum(node->left());
				}
				else
				{
					// if its children have no value smaller than the val it has,
					// then a smaller value can only be held by some of its parent who's bigger than the branch contains it
					// (i.e. it or its some parent is a right child of a node)

					binary_node<T>* child, parent = node;
					do
					{
						child = parent;
						parent = parent->p;
					} while (parent != nullptr && child == parent->left()); // go back to its parent until the parent is nullptr or until we have a right child

					return iterator(parent);
				}
			}

			iterator& operator++() { node = successor().node; return *this; }
			iterator operator++(int) { iterator temp = iterator(node); node = successor().node; return temp; }
			iterator& operator--() { node = predecessor().node; return *this; }
			iterator operator--(int) { iterator temp = iterator(node); node = predecessor().node; return temp; }

			binary_node<T>* operator->() const { return node; }
			binary_node<T>& operator*() const { return *node; }

			binary_node<T>* node = nullptr;
		};

		iterator get_root_iter() const { return iterator(this->root.get()); }

	public:
		/*
		* Insert the element pointed to by ele who owns the element.
		* After this, the ownership is taken.
		*
		* @returns the iterator to the newly inserted element
		*/
		iterator insert(T* ele)
		{
			binary_node<T>* x = get_root_iter();
			binary_node<T>* y = nullptr; // parent of x
			binary_node<T>* res = nullptr;

			while (x != nullptr)
			{
				y = x;

				if (*ele <= x->get_obj())
				{
					x = x->left();
				}
				else
				{
					x = x->right();
				}
			}

			if (nullptr == y) // root is nullptr, the tree is empty
			{
				this->root = (res = new binary_node<T>(nullptr, ele));
			}
			else
			{
				if (*ele <= y->get_obj())
				{
					y->set_left(res = new binary_node<T>(y, ele));
				}
				else
				{
					y->set_right(res = new binary_node<T>(y, ele));
				}
			}

			return iterator(res);
		}

		/*
		* Removes the element at pos and rearranges the tree so that the property holds
		*/
		void remove(iterator pos)
		{
			binary_node<T>* node = pos.node;
			/*
			* 1. node has no children. replace it with nullptr in its parent
			* 2. node has only 1 child. replace it with its only child in its parent.
			* 3. node has 2 children. replace it with its successor, and rearrange the tree.
			*/

			if (node->left() == nullptr) // if left is empty, then whether right is empty or not, we can replace node with its right anyway
			{
				transplant(node, node->right());
			}
			else if (node->right() == nullptr) // right is empty
			{
				transplant(node, node->left());
			}
			else // both children exist
			{
				binary_node<T>* successor = internal_minimum(node->right());

				if (node->right() != successor) // the successor is not node->right
				{
					// first remember successor's parent
					binary_node<T>* ps = successor->p;

					// the successor must be in a left subtree.
					// otherwise, it is bigger than its parent, contradicting to it being the successor
					// now we take ownership of it because we are going to transplant it.
					successor = successor->p->branches[0].release();
					// successor is dangling now. Always set a dangling node's parent to nullptr
					successor->p = nullptr;

					// and we plant the successor's right to successor's previous place (now nullptr)
					transplant(ps->left(), successor->right());

					// because successor's left must be nullptr,
					// after the transplanting, successor has no child.
					// now we transplant node.right to successor.right
					// and transplant node.left to successor.left
					transplant(successor->right(), node->right());
					transplant(successor->left(), node->left());
					
					// now successor will take node's place
					transplant(node, successor);
				}
				else // the successor is node->right
				{
					// we plant node->left to successor's
					transplant(successor->left(), node->left());

					// and now successor will take node's place
					transplant(node, successor);
				}
			}
		}

		/*
		* Removes the element if it is present
		* 
		* @returns true iff the element is removed
		*/
		bool remove(const T& ele) 
		{
			auto i = find(ele);
			if (i.valid())
			{
				remove(i);
				return true;
			}
			else
			{
				return false;
			}
		}
		/*
		* Removes the element if it is present
		*
		* @param k the key that represents the element
		* @returns true iff the element is removed
		*/
		template <typename Key>
		bool remove(Key k)
		{
			auto i = find(k);
			if (i.valid())
			{
				remove(i);
				return true;
			}
			else
			{
				return false;
			}
		}

		/*
		* @returns the iterator to ele if it's present, or otherwise an invalid iter
		*/
		iterator find(const T& ele) const { return iterator(internal_find(get_root_iter(), ele)); }
		/*
		* @returns the iterator to the element represented by k if it's present, or otherwise an invalid iter
		*/
		template <typename Key>
		iterator find(Key k) const { return iterator(internal_find(get_root_iter(), k)); }

		iterator maximum() const { return internal_maximum(get_root_iter()); }
		iterator minimum() const { return internal_minimum(get_root_iter()); }

	private:
		// where a reference to the current node is needed, 
		// we write these internal methods that take an extra argument as the node to do the actual works

		binary_node<T>* internal_find(binary_node<T>* node, const T& ele) const
		{
			while (node != nullptr && node->p_obj != nullptr)
			{
				if (node->get_obj() == ele)
				{
					return node;
				}
				else
				{
					if (ele <= node->get_obj())
					{
						node = node->left();
					}
					else // ele >= node->get_obj() as it's a total ordering
					{
						node = node->right();
					}
				}
			}
		}
		template <typename Key>
		binary_node<T>* internal_find(binary_node<T>* node, Key k) const
		{
			while (node != nullptr && node->p_obj != nullptr)
			{
				if (node->get_obj() == k)
				{
					return node;
				}
				else
				{
					if (node->get_obj() <= k)
					{
						node = node->right();
					}
					else // node->get_obj() >= k as it's a total ordering
					{
						node = node->left();
					}
				}
			}
		}

		binary_node<T>* internal_maximum(binary_node<T>* node) const
		{
			while (node.right() != nullptr)
			{
				node = node.right();
			}

			return node;
		}
		binary_node<T>* internal_minimum(binary_node<T>* node) const
		{
			while (node.left() != nullptr)
			{
				node = node.left();
			}

			return node;
		}

		/*
		* replaces the subtree at u with the subtree at v.
		* the ownership of v will be given from v's previous parent to u's parent.
		* after it, the subtree that was at u is destroyed, and the place where v was at now becomes nullptr
		* 
		* u must not be a child of v, or the behavior is undefined otherwise.
		*/
		void transplant(binary_node<T>* u, binary_node<T>* v)
		{
			if (v == nullptr)
			{
				if (u->p == nullptr) // u is the root
				{
					this->root.reset(v);
				}
				else
				{
					if (u == u->p->left())
					{
						u->p->set_left(v);
					}
					else
					{
						u->p->set_right(v);
					}
				}
			}
			else
			{
				if (v->p != nullptr)
				{
					// first releases the ownership of v's parent to v
					// to avoid multiple ownerships or 0 ownership
					if (v == v->p->left())
					{
						v = v->p->branches[0].release();
					}
					else
					{
						v = v->p->branches[1].release();
					}
				}

				// rebind v's parent
				v->p = u->p;

				if (u->p == nullptr) // u is the root
				{
					this->root.reset(v);
				}
				else
				{
					if (u == u->p->left())
					{
						u->p->set_left(v);
					}
					else
					{
						u->p->set_right(v);
					}
				}			
			}
		}
	};
}