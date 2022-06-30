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
	* It is an interface that currently provides the basic methods of a tree and interactions with the object stored at root
	* 
	* @param T: type of objects stored in the tree
	*/
	template <typename T>
	class tree
	{
	public:
		// takes ownership of ptr
		explicit tree(T* ptr = nullptr, tree* parent = nullptr) : p_obj(ptr), p(parent) {}
		// other relinquishes the ownership to this.
		tree(tree&& other) : p_obj(std::move(other.p_obj)), p(other.p) {}
		// intend to allow inheritance
		virtual ~tree() {}

		tree& operator=(tree&& right) noexcept
		{
			p_obj = std::move(right.p_obj);
			p = right.p;
			return *this;
		}

	public:
		std::unique_ptr<T> p_obj;
		// direct parent
		tree* p;

	public:
		// @returns ture iff the obj is valid
		bool object_valid() const { return p_obj != nullptr; }

		bool has_parent() const { return nullptr != p; }
		// for derived classes of tree, the parent also becomes the derived type. 
		// use this to cast the parent to any derived type
		template <typename U = tree<T>>
		U* get_parent() const { return static_cast<U*>(p); }

		// @returns true iff the tree is empty
		virtual bool empty() const = 0;

		// resets the object stored at root (destructs it and sets the pointer to ptr)
		// by default, it's reset to nullptr
		void reset_object(T* ptr = nullptr) { p_obj->reset(ptr); }

		// @returns the object stored at root
		T& get_obj() { return *p_obj; }
		const T& get_obj() const { return *p_obj; }
	};

	/*
	* functions fpr traversing a tree
	* 
	* The trees to be traversed by these functions must support iteration through its branches
	* by supporting the ranged-for loop
	*/ 

	/*
	* Traverse the tree on given type with listener
	*
	* for now, only define traversal for preorder, postorder, and breadth-first.
	* inorder traversal will be defined in the binary tree specialization
	*/
	template <typename T, template<typename > class TREE>
	inline void traverse(tree_traversal_listener<T>& listener, tree_traversal_type type, TREE<T> & t)
	{
		if (tree_traversal_type::preorder == type)
		{
			traverse_node_preorder(listener, t);
		}
		else if (tree_traversal_type::postorder == type)
		{
			traverse_node_postorder(listener, t);
		}
		else if (tree_traversal_type::breadth_first == type)
		{
			traverse_node_breadth_first(listener, t);
		}
	}

	/*
	* Recursively traverses the node and its branches in preorder
	*/
	template <typename T, template<typename > class TREE>
	void traverse_node_preorder(tree_traversal_listener<T>& listener, TREE<T>& t)
	{
		listener.enter_node(t.get_obj());
		listener.exit_node(t.get_obj());

		// iterate through branches
		for (const auto& b : t)
		{
			traverse_node_preorder(listener, b);
		}
	}
	/*
	* Recursively traverses the node and its branches in postorder
	*/
	template <typename T, template<typename > class TREE>
	void traverse_node_postorder(tree_traversal_listener<T>& listener, TREE<T>& t)
	{
		// iterate through branches
		for (const auto& b : t)
		{
			traverse_node_postorder(listener, b);
		}

		listener.enter_node(t.get_obj());
		listener.exit_node(t.get_obj());
	}
	/*
	* Traverses the node and its branches breadth-firstly.
	*
	* This cannot be done recursively, as a node is not aware of nodes in the same level as it is.
	*/
	template <typename T, template<typename> class TREE>
	void traverse_node_breadth_first(tree_traversal_listener<T>& listener, TREE<T>& t)
	{
		// traverse the queue from front to back
		ghl::list<const TREE<T>*> traversal_queue{ &t };

		/*
		* Traverse front, putting all its direct branches at back, and remove front
		*/
		while (!traversal_queue.empty())
		{
			const auto* n = traversal_queue.front();

			listener.enter_node(n->get_obj());
			listener.exit_node(n->get_obj());

			// iterate through branches
			for (const auto& b : *n)
			{
				traversal_queue.insert_back(&b);
			}

			traversal_queue.remove_front();
		}
	}

	/*
	* tree that has fixed number of branches
	* 
	* T must be constructiable and destructiable
	*/
	template <typename T, size_t N>
	class fixed_branch_size_tree : public tree<T>
	{
	public:
		using obj_t = T;

	public:
		/*
		* Init obj and take ownership of in_branches.
		* In addtion, the parents of in_brances will be set to this
		*/
		explicit fixed_branch_size_tree(T* obj = nullptr, fixed_branch_size_tree* parent = nullptr, fixed_branch_size_tree* in_branches[N] = { nullptr }) :
			tree<T>(obj, parent)
		{
			for (size_t i = 0; i != N; ++i)
			{
				if (in_branches[i] != nullptr)
				{
					++num_non_empty_branches;
				}

				branches[i].reset(in_branches[i]);
			}

			rebind_parent();
		}
		// we cannot assume we can copy the obj
		fixed_branch_size_tree(const fixed_branch_size_tree& other) = delete;
		fixed_branch_size_tree(fixed_branch_size_tree&& other) : 
			tree(std::move(other)),
			num_non_empty_branches(other.num_non_empty_branches)
		{
			for (size_t i = 0; i != N; ++i)
			{
				branches[i] = std::move(other.branches[i]);
			}
			rebind_parent();
		}
		fixed_branch_size_tree& operator=(fixed_branch_size_tree&& right)
		{
			*static_cast<tree<T>*>(this) = static_cast<tree<T>&&>(right);
			num_non_empty_branches = std::move(right.num_non_empty_branches);

			for (size_t i = 0; i != N; ++i)
			{
				branches[i] = std::move(right.branches[i]);
			}
			rebind_parent();

			return *this;
		}

		virtual ~fixed_branch_size_tree() {}

	public:
		static constexpr size_t num_branches() { return N; }

		/*
		* @returns true iff the branch at ind is empty
		*/
		bool branch_empty(size_t ind) const { return nullptr == branches[ind]; }

		/*
		* @returns true iff the tree is empty
		* (i.e. all branches are empty and the node contains no element)
		*/
		bool empty() const override { return 0 == num_non_empty_branches && !(this->object_valid()); }

		/*
		* Sets the branch at ind
		*/
		inline void set_branch(size_t ind, fixed_branch_size_tree* branch)
		{
			if(nullptr == branches[ind]) ++num_non_empty_branches;
			else branch->p = this;
			branches[ind] = std::unique_ptr<fixed_branch_size_tree>(branch);
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
		using iterator = std::unique_ptr<fixed_branch_size_tree>*;
		using const_iterator = const std::unique_ptr<fixed_branch_size_tree>*;

		iterator begin() { return &branches[0]; }
		iterator end() { return &branches[0] + N; }
		const_iterator begin() const { return &branches[0]; }
		const_iterator end() const { return &branches[0] + N; }
		const_iterator cbegin() const { return &branches[0]; }
		const_iterator cend() const { return &branches[0] + N; }

	protected:
		/*
		* rebinds the parent of all present branches to this
		*/
		void rebind_parent() 
		{
			for (auto& b : branches)
			{
				if (b != nullptr) b->p = this;
			}
		}
		
	public:
		std::unique_ptr<fixed_branch_size_tree> branches[N];

		// number of NON-EMPTY branches
		size_t num_non_empty_branches;
	};

	template <typename T>
	using binary_tree = fixed_branch_size_tree<T, 2>;

	// partial specialization for binary trees
	template <typename T>
	class fixed_branch_size_tree<T, 2> : public tree<T>
	{
	public:
		using obj_t = T;

	public:
		/*
		* Init obj and take ownership of in_branches
		*/
		template <typename... P>
		fixed_branch_size_tree(T* obj = nullptr, fixed_branch_size_tree* parent = nullptr, fixed_branch_size_tree* in_left = nullptr, fixed_branch_size_tree* in_right = nullptr) :
			tree<T>(obj, parent)
		{
			branches[0].reset(in_left); branches[1].reset(in_right);
			rebind_parent();
		}
		// we cannot assume we can copy the obj
		fixed_branch_size_tree(const fixed_branch_size_tree& other) = delete;
		fixed_branch_size_tree(fixed_branch_size_tree&& other) : tree(std::move(other))
		{
			for (size_t i = 0; i != 2; ++i)
			{
				branches[i] = std::move(other.branches[i]);
			}
			rebind_parent();
		}
		fixed_branch_size_tree& operator=(fixed_branch_size_tree&& right)
		{
			*static_cast<tree<T>*>(this) = static_cast<tree<T>&&>(right);

			for (size_t i = 0; i != 2; ++i)
			{
				branches[i] = std::move(right.branches[i]);
			}
			rebind_parent();

			return *this;
		}

		virtual ~fixed_branch_size_tree() {}

	public:
		static constexpr size_t num_branches() { return 2; }

		/*
		* @returns true iff the left branch is empty
		*/
		bool left_empty() const { return nullptr == branches[0] ? true : branches[0]->empty(); }
		/*
		* @returns true iff the right branch is empty
		*/
		bool right_empty() const { return nullptr == branches[1] ? true : branches[1]->empty(); }

		// @returns true iff the tree has a left (i.e. it's not nil)
		bool has_left() const { return nullptr != left(); }
		// @returns true iff the tree has a right (i.e. it's not nil)
		bool has_right() const { return nullptr != right(); }

		/*
		* @returns true iff the tree is empty
		* (i.e. all branches are empty and the node contains no element)
		*/
		inline bool empty() const override { return left_empty() && right_empty() && !(this->object_valid()); }

	public:
		template <typename U = fixed_branch_size_tree>
		U* left() { return static_cast<U*>(branches[0].get()); }
		template <typename U = fixed_branch_size_tree>
		U* right() { return static_cast<U*>(branches[1].get()); }
		template <typename U = fixed_branch_size_tree>
		const U* left() const { return static_cast<const U*>(branches[0].get()); }
		template <typename U = fixed_branch_size_tree>
		const U* right() const { return static_cast<const U*>(branches[1].get()); }

		/*
		* Releases the ownership of the left branch and returns a pointer to it.
		* In addition, it resets the left branch's parent to nullptr
		* 
		* After which, the tree no longer stores the previous left pointer and it's left becomes nullptr.
		*/
		template <typename U = fixed_branch_size_tree>
		U* release_left() { auto res = branches[0].release(); if(res) res->p = nullptr; return static_cast<U*>(res); }
		/*
		* The mirror function of release_left
		*/
		template <typename U = fixed_branch_size_tree>
		U* release_right() { auto res = branches[1].release(); if(res) res->p = nullptr; return static_cast<U*>(res); }

		/*
		* Sets the left branch to n (potentially releases the present left if existing),
		* and sets n's parent to this
		*/
		virtual void set_left(fixed_branch_size_tree* n) { if (n) n->p = this; branches[0].reset(n); }
		/*
		* The mirror function of set_left
		*/
		virtual void set_right(fixed_branch_size_tree* n) { if (n) n->p = this; branches[1].reset(n); }
		// do not make the following two virtual
		// as derived classes may have different arguments for their constructors
		void emplace_left(T* p_obj = nullptr, fixed_branch_size_tree* parent = nullptr, fixed_branch_size_tree * in_left = nullptr, fixed_branch_size_tree * in_right = nullptr) { set_left(new fixed_branch_size_tree(p_obj, parent, left, right)); }
		void emplace_right(T* p_obj = nullptr, fixed_branch_size_tree* parent = nullptr, fixed_branch_size_tree* in_left = nullptr, fixed_branch_size_tree* in_right = nullptr) { set_right(new fixed_branch_size_tree(p_obj, parent, left, right)); }
		
		virtual void reset_left() { branches[0].reset(); }
		virtual void reset_right() { branches[1].reset(); }

		/*
		* O(h), where h is height
		* 
		* @returns true if node is on the left subtree or false if it is on the right.
		* However, if node is root or if node isn't on this tree, the behaviour is undefined
		*/
		bool is_on_left_or_right(const fixed_branch_size_tree* node) const
		{
			const fixed_branch_size_tree* x = node, * y = node->get_parent<fixed_branch_size_tree>();

			while (y != this)
			{
				x = y; y = x->get_parent<fixed_branch_size_tree>();
			}

			return x == left();
		}

	protected:
		/*
		* rebinds the parent of all present branches to this
		*/
		void rebind_parent()
		{
			for (auto& b : branches)
			{
				if (b != nullptr) b->p = this;
			}
		}

	public:
		using iterator = std::unique_ptr<fixed_branch_size_tree>*;
		using const_iterator = const std::unique_ptr<fixed_branch_size_tree>*;

		iterator begin() { return &branches[0]; }
		iterator end() { return &branches[0] + 2; }
		const_iterator begin() const { return &branches[0]; }
		const_iterator end() const { return &branches[0] + 2; }
		const_iterator cbegin() const { return &branches[0]; }
		const_iterator cend() const { return &branches[0] + 2; }

	public:
		// still declare it as an array so that left and right are continuous in memory
		std::unique_ptr<fixed_branch_size_tree> branches[2];
	};

	// binary tree with an additional attribute that equals to it's height
	template <typename T>
	class binary_tree_with_height final : public binary_tree<T>
	{
	private:
		using super = binary_tree<T>;

	public:
		binary_tree_with_height
		(
			T* p_obj = nullptr,
			binary_tree_with_height* parent = nullptr,
			binary_tree_with_height* left = nullptr,
			binary_tree_with_height* right = nullptr
		) :
			binary_tree<T>(p_obj, parent, left, right)
		{
			update_height_on_path(this);
		}
		binary_tree_with_height(binary_tree_with_height && other) :
			binary_tree<T>(static_cast<binary_tree<T>&&>(other)),
			height(other.height) {}

	public:
		void set_left(super* n) override { this->super::set_left(n); update_height_on_path(this->left<binary_tree_with_height>()); }
		void set_right(super* n) override { this->super::set_right(n); update_height_on_path(this->right<binary_tree_with_height>()); }
		void emplace_left(T* p_obj = nullptr, binary_tree_with_height* parent = nullptr, binary_tree_with_height* left = nullptr, binary_tree_with_height* right = nullptr) { set_left(new binary_tree_with_height(p_obj, parent, left, right)); }
		void emplace_right(T* p_obj = nullptr, binary_tree_with_height* parent = nullptr, binary_tree_with_height* left = nullptr, binary_tree_with_height* right = nullptr) { set_right(new binary_tree_with_height(p_obj, parent, left, right)); }
		void reset_left() override { this->super::reset_left(); /* now it's branch is reset, the path end becomes this */ update_height_on_path(this); }
		void reset_right() override { this->super::reset_right(); /* now it's branch is reset, the path end becomes this */ update_height_on_path(this); }
		binary_tree_with_height* release_left() { auto res = this->super::release_left<binary_tree_with_height>(); update_height_on_path(this); return res; }
		binary_tree_with_height* release_right() { auto res = this->super::release_right<binary_tree_with_height>(); update_height_on_path(this); return res; }

		auto get_height() const { return height; }

	private:
		// recalculates its height based on the branches' heights
		void update_height()
		{
			bool le = this->left_empty(), re = this->right_empty();

			if (le) // max = right's height or 0 if right is empty
			{
				height = 1 + (re ? 0 : this->right<binary_tree_with_height>()->height);
			}
			else if (re) // max = left's height
			{
				height = 1 + this->left<binary_tree_with_height>()->height;
			}
			else
			{
				height = 1 + std::max(this->right<binary_tree_with_height>()->height, this->left<binary_tree_with_height>()->height);
			}
		}
		/*
		* when a new node is constructed or an old one is updated, it is likely to have its height constructed/changed, 
		* and thus is likely to change the heights along the path to root
		* 
		* Call this to update all heights along the path from root to path_end
		*/
		static void update_height_on_path(binary_tree_with_height* path_end)
		{
			if (path_end)
			{
				binary_tree_with_height* x = path_end, * y = path_end->get_parent<binary_tree_with_height>();

				// do it for the end first so every node on the path will be updated.
				x->update_height();

				while (y != nullptr) // until x is root and does it one last time
				{
					y->update_height();

					x = y;
					y = x->get_parent<binary_tree_with_height>();
				}
			}
		}

		unsigned height;
	};

	// tree that can contain arbitrary number of branches
	template <typename T>
	class general_tree : public tree<T>
	{
	private:
		using container_t = ghl::list<general_tree>;
	public:
		using obj_t = T;

	public:
		size_t num_branches() const { return branches.size(); }

	public:
		using iterator = typename container_t::iterator;
		using const_iterator = typename container_t::const_iterator;

		iterator begin() { return branches.begin(); }
		iterator end() { return branches.end(); }
		const_iterator cbegin() const { return branches.cbegin(); }
		const_iterator cend() const { return branches.cend(); }

	private:
		ghl::list<general_tree> branches;
	};
}