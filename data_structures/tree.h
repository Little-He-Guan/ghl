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
		tree() : p_obj(nullptr) {}
		// takes ownership of ptr
		tree(T* ptr) : p_obj(ptr) {}
		// other relinquishes the ownership to this.
		tree(tree&& other) : p_obj(std::move(other.p_obj)) {}
		// intend to allow inheritance
		virtual ~tree() {}

		tree& operator=(tree&& right) noexcept
		{
			p_obj = std::move(right.p_obj);
			return *this;
		}

	protected:
		std::unique_ptr<T> p_obj;

	public:
		// @returns ture iff the obj is valid
		bool object_valid() const { return p_obj != nullptr; }

		// @returns true iff the tree is empty
		virtual bool empty() const = 0;

		// resets the object stored at root (destructs it and sets the pointer to nullptr)
		void reset_object() { p_obj->reset(nullptr); }

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
		* Init obj and take ownership of in_branches
		*/
		explicit fixed_branch_size_tree(fixed_branch_size_tree* parent = nullptr, T* obj = nullptr, fixed_branch_size_tree* in_branches[N] = {}) :
			tree<T>(obj),
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
		fixed_branch_size_tree(const fixed_branch_size_tree& other) = delete;
		fixed_branch_size_tree(fixed_branch_size_tree&& other)
			: 
			tree(std::move(other)), p(other.p),
			num_non_empty_branches(other.num_non_empty_branches)
		{
			for (size_t i = 0; i != N; ++i)
			{
				branches[i] = std::move(other.branches[i]);
			}
		}
		fixed_branch_size_tree& operator=(fixed_branch_size_tree&& right)
		{
			*static_cast<tree<T>*>(this) = static_cast<tree<T>&&>(right);
			p = right.p;
			num_non_empty_branches = right.num_non_empty_branches;

			for (size_t i = 0; i != N; ++i)
			{
				branches[i] = std::move(right.branches[i]);
			}

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
		* Sets the branch at ind iff it is empty
		*/
		inline void set_branch(size_t ind, fixed_branch_size_tree* branch)
		{
			if (nullptr == branches[ind])
			{
				++num_non_empty_branches;
				branches[ind] = std::unique_ptr<fixed_branch_size_tree>(branch);
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
		using iterator = std::unique_ptr<fixed_branch_size_tree>*;
		using const_iterator = const std::unique_ptr<fixed_branch_size_tree>*;

		iterator begin() { return &branches[0]; }
		iterator end() { return &branches[0] + N; }
		const_iterator begin() const { return &branches[0]; }
		const_iterator end() const { return &branches[0] + N; }
		const_iterator cbegin() const { return &branches[0]; }
		const_iterator cend() const { return &branches[0] + N; }

	public:
		std::unique_ptr<fixed_branch_size_tree> branches[N];

		// number of NON-EMPTY branches
		size_t num_non_empty_branches;

	public:
		// its direct parent
		fixed_branch_size_tree* p = nullptr;
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
		fixed_branch_size_tree(fixed_branch_size_tree* parent = nullptr, T* obj = nullptr, fixed_branch_size_tree* in_left = nullptr, fixed_branch_size_tree* in_right = nullptr) :
			tree<T>(obj),
			p(parent)
		{
			branches[0].reset(in_left); branches[1].reset(in_right);
		}
		// we cannot assume we can copy the obj
		fixed_branch_size_tree(const fixed_branch_size_tree& other) = delete;
		fixed_branch_size_tree(fixed_branch_size_tree&& other) : tree(std::move(other)), p(other.p)
		{
			for (size_t i = 0; i != 2; ++i)
			{
				branches[i] = std::move(other.branches[i]);
			}
		}
		fixed_branch_size_tree& operator=(fixed_branch_size_tree&& right)
		{
			*static_cast<tree<T>*>(this) = static_cast<tree<T>&&>(right);
			p = right.p;

			for (size_t i = 0; i != 2; ++i)
			{
				branches[i] = std::move(right.branches[i]);
			}

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

		/*
		* @returns true iff the tree is empty
		* (i.e. all branches are empty and the node contains no element)
		*/
		inline bool empty() const override { return left_empty() && right_empty() && !(this->object_valid()); }

	public:
		fixed_branch_size_tree* left() { return branches[0].get(); }
		fixed_branch_size_tree* right() { return branches[1].get(); }
		const fixed_branch_size_tree* left() const { return branches[0].get(); }
		const fixed_branch_size_tree* right() const { return branches[1].get(); }

		void set_left(fixed_branch_size_tree* n) { branches[0].reset(n); }
		void set_right(fixed_branch_size_tree* n) { branches[1].reset(n); }
		void reset_left() { branches[0].reset(); }
		void reset_right() { branches[1].reset(); }

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

	public:
		// its direct parent
		fixed_branch_size_tree* p = nullptr;
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