#pragma once

#include "tree.h"

namespace ghl
{

	/*
	* T must have a total ordering imposed by operator<=, and equality imposed by operator==
	* If T can be represented by light weight key, then T must also have operator<= and operator== that take a key
	*
	* Invariant: the binary search tree property.
	* 
	* Note: the class manages the elements by holding a **container tree**, whose type is the template parameter Container
	* By default it's the binary tree.
	* Under some conditions, it's better to use other special trees (e.g. a tree whose nodes have an additional size attribute to implement an AVL tree)
	* It is presumed that the container at least supports the same set of operations as a binary_tree does.
	*/
	template <typename T, template <typename> class Container = binary_tree>
	class binary_search_tree
	{
	public:
		using node_t = Container<T>;

	public:
		// inits root
		binary_search_tree() : root(nullptr) {}
		explicit binary_search_tree(node_t* rt) : root(rt) {}
		/*
		* Assumes that the pointers own the elements
		* After it, the ownerships will be taken
		*/
		binary_search_tree(const std::initializer_list<T*>& init_list) : binary_search_tree()
		{
			for (auto* p : init_list)
			{
				insert(p);
			}
		}

		virtual ~binary_search_tree() {}

	protected:
		std::unique_ptr<Container<T>> root;

	public:
		node_t* get_root() const { return root.get(); }

		void set_root(node_t* new_root) { root.reset(new_root); }

	public:
		struct iterator
		{
			iterator() {}
			explicit iterator(node_t* n) : node(n) {}
			iterator(const iterator& o) : node(o.node) {}
			iterator(iterator&& o) : node(o.node) {}
			~iterator() {}

			iterator& operator=(const iterator& r) { node = r.node; return *this; }
			iterator& operator=(iterator&& r) { node = r.node; return *this; }

			bool valid() const { return node != nullptr ? (!node->empty()) : false; }

			// @returns the iterator to the successor of node if it exists in the tree, or an invalid iter otherwise
			iterator successor() const
			{
				if (node->has_right())
				{
					return iterator(binary_search_tree::internal_minimum(node->right<Container<T>>()));
				}
				else
				{
					// if its children have no value bigger than the val it has,
					// then a bigger value can only be held by some of its parent who's bigger than the branch contains it
					// (i.e. it or its some parent is a left child of a node)

					Container<T>* child, *parent = node;
					do
					{
						child = parent;
						parent = parent->get_parent<Container<T>>();
					} while (parent != nullptr && child == parent->right<Container<T>>()); // go back to its parent until the parent is nullptr or until we have a left child

					// if parent is not nullptr, it is the first parent that has a left child in the branch
					// by the property, it is the smallest one bigger than this
					// or otherwise, there are no bigger val in the tree than this, and we return parent = nullptr;
					return iterator(parent);
				}
			}
			// @returns the iterator to the predecessor of node if it exists in the tree, or an invalid iter otherwise
			iterator predecessor() const
			{
				if (node->has_left())
				{
					return iterator(binary_search_tree::internal_maximum(node->left<Container<T>>()));
				}
				else
				{
					// if its children have no value smaller than the val it has,
					// then a smaller value can only be held by some of its parent who's bigger than the branch contains it
					// (i.e. it or its some parent is a right child of a node)

					Container<T>* child, *parent = node;
					do
					{
						child = parent;
						parent = parent->get_parent<Container<T>>();
					} while (parent != nullptr && child == parent->left<Container<T>>()); // go back to its parent until the parent is nullptr or until we have a right child

					return iterator(parent);
				}
			}

			iterator& operator++() { node = successor().node; return *this; }
			iterator operator++(int) { iterator temp = iterator(node); node = successor().node; return temp; }
			iterator& operator--() { node = predecessor().node; return *this; }
			iterator operator--(int) { iterator temp = iterator(node); node = predecessor().node; return temp; }

			Container<T>* operator->() const { return node; }
			Container<T>& operator*() const { return *node; }

			Container<T>* node = nullptr;
		};

		iterator get_root_iter() const { return iterator(root.get()); }

	public:
		/*
		* Insert the element pointed to by ele who owns the element.
		* After this, the ownership is taken.
		* 
		* @param Note: if duplication is allowed in any insertion, then it cannot be turned off all insertions afterwards (the function will not act according to bAllowDuplication afterwards).
		* As a consequence, it is strongly recommended that it is always allowed or not allowed for any single tree
		*
		* @returns the iterator to the newly inserted element
		*/
		virtual iterator insert(T* ele, bool bAllowDuplication = true)
		{
			Container<T>* x = root.get();
			Container<T>* y = nullptr; // parent of x
			Container<T>* res = nullptr;

			while (x != nullptr)
			{
				y = x;

				if (*ele <= x->get_obj())
				{
					x = x->left<Container<T>>();
				}
				else
				{
					x = x->right<Container<T>>();
				}
			}
			if (nullptr == y) // the tree is empty
			{
				root.reset(new Container<T>(ele));
				res = root.get();
			}
			else
			{
				if (bAllowDuplication)
				{
					if (*ele <= y->get_obj())
					{
						y->set_left(res = new Container<T>(ele, y));
					}
					else
					{
						y->set_right(res = new Container<T>(ele, y));
					}
				}
				else
				{
					if (*ele < y->get_obj())
					{
						y->set_left(res = new Container<T>(ele, y));
					}
					else if(*ele > y->get_obj())
					{
						y->set_right(res = new Container<T>(ele, y));
					}
				}
			}

			return iterator(res);
		}

		/*
		* Removes the element at pos and rearranges the tree so that the property holds
		* 
		* Note: if pos is valid but is not in the tree, the behaviour is undefined.
		* @returns true iff pos is valid and is removed
		*/
		virtual bool remove(iterator pos)
		{
			if (pos.valid())
			{
				internal_remove(pos.node);
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
		* @returns true iff the element is removed
		*/
		bool remove(const T& ele) { return remove(find(ele)); }
		/*
		* Removes the element if it is present
		*
		* @param k the key that represents the element
		* @returns true iff the element is removed
		*/
		template <typename Key>
		bool remove(Key k) { return remove(find(k)); }

		/*
		* @returns the iterator to ele if it's present, or otherwise an invalid iter
		*/
		iterator find(const T& ele) const { return iterator(internal_find(get_root(), ele)); }
		/*
		* @returns the iterator to the element represented by k if it's present, or otherwise an invalid iter
		*/
		template <typename Key>
		iterator find(Key k) const { return iterator(internal_find(get_root(), k)); }

		iterator maximum() const { return iterator(internal_maximum(get_root())); }
		iterator minimum() const { return iterator(internal_minimum(get_root())); }

	protected:
		// where a reference to the current node is needed, 
		// we write these internal methods that take an extra argument as the node to do the actual works

		static Container<T>* internal_find(Container<T>* node, const T& ele)
		{
			while (node != nullptr && node->object_valid())
			{
				if (node->get_obj() == ele)
				{
					return node;
				}
				else
				{
					if (ele <= node->get_obj())
					{
						node = node->left<Container<T>>();
					}
					else // ele >= node->get_obj() as it's a total ordering
					{
						node = node->right<Container<T>>();
					}
				}
			}

			return node;
		}
		template <typename Key>
		static Container<T>* internal_find(Container<T>* node, Key k)
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
						node = node->right<Container<T>>();
					}
					else // node->get_obj() >= k as it's a total ordering
					{
						node = node->left<Container<T>>();
					}
				}
			}

			return node;
		}

		static Container<T>* internal_maximum(Container<T>* node)
		{
			while (node->right<Container<T>>() != nullptr)
			{
				node = node->right<Container<T>>();
			}

			return node;
		}
		static Container<T>* internal_minimum(Container<T>* node)
		{
			while (node->left<Container<T>>() != nullptr)
			{
				node = node->left<Container<T>>();
			}

			return node;
		}

		/*
		* replaces the subtree at u with the subtree at v.
		* the ownership of v will be given from v's previous parent to u's parent.
		* after it, the subtree that was at u is destroyed, and the place where v was at now becomes nullptr
		*
		* @param u must not be a child of v, or the behavior is undefined.
		* In addition, if u is nullptr, then its parent cannot be traced back,
		* so we use an additional parameter that points to it's parent: up.
		* 
		* @param However, it also makes it impossible to tell if u was up's left or right.
		* We then add another parameter that tells if u was left or right (true for left, false for right).
		* If up is nullptr, this parameter is ignored.
		*/
		void transplant(Container<T>* u, Container<T>* v, Container<T>* up, bool b_u_left_right)
		{
			if (v == nullptr)
			{
				if (up == nullptr) // u is the root
				{
					this->root.reset(nullptr);
				}
				else
				{
					if (b_u_left_right)
					{
						up->set_left(nullptr);
					}
					else
					{
						up->set_right(nullptr);
					}
				}
			}
			else
			{
				if (v->has_parent())
				{
					// first releases the ownership of v's parent to v
					// to avoid multiple ownerships or 0 ownership
					if (v == v->get_parent<Container<T>>()->left<Container<T>>())
					{
						v = v->get_parent<Container<T>>()->release_left();
					}
					else
					{
						v = v->get_parent<Container<T>>()->release_right();
					}
				}

				// rebind v's parent
				v->p = up;

				if (up == nullptr) // u is the root
				{
					this->root.reset(v);
				}
				else
				{
					if (b_u_left_right)
					{
						up->set_left(v);
					}
					else
					{
						up->set_right(v);
					}
				}
			}
		}

		/*
		* Note: it assumes that node is not nullptr
		* 
		* @returns the pointer to the node that will be in the place where the node to be deleted is now, or
		* the parent to the place, if the place will become nullptr, or
		* nullptr if both of the two will be nullptr (that is, when the tree will be empty)
		*/
		node_t* internal_remove(node_t* node)
		{
			node_t* res;

			// tells if node is its parent's left or right, used in transparent
			// if node->p is nullptr, then it will be ignored by transplant, and we simply set it to ture then.
			bool b_node_left_right = node->has_parent() ? node == node->get_parent<Container<T>>()->left<Container<T>>() : true;

			/*
			* 1. node has no children. replace it with nullptr in its parent
			* 2. node has only 1 child. replace it with its only child in its parent.
			* 3. node has 2 children. replace it with its successor, and rearrange the tree.
			*/

			if (!node->has_left()) // if left is nil, then whether right is nil or not, we can replace node with its right anyway
			{
				res = node->has_right() ? node->right<Container<T>>() : node->get_parent<Container<T>>();
				transplant(node, node->right<Container<T>>(), node->get_parent<Container<T>>(), b_node_left_right);
			}
			else if (!node->has_right()) // right is nil
			{
				res = node->has_left() ? node->left<Container<T>>() : node->get_parent<Container<T>>();
				transplant(node, node->left<Container<T>>(), node->get_parent<Container<T>>(), b_node_left_right);
			}
			else // both children exist
			{
				Container<T>* successor = internal_minimum(node->right<Container<T>>());
				res = successor != nullptr ? successor : node->get_parent<Container<T>>();

				if (node->right<Container<T>>() != successor) // the successor is not node->right
				{
					// first remember successor's parent
					Container<T>* ps = successor->get_parent<Container<T>>();

					// the successor must be in a left subtree.
					// otherwise, it is bigger than its parent, contradicting to it being the successor
					// now we take ownership of it because we are going to transplant it.
					successor = successor->get_parent<Container<T>>()->release_left();

					// and we plant the successor's right to successor's previous place (now nullptr)
					transplant(ps->left<Container<T>>(), successor->right<Container<T>>(), ps, true);

					// because successor's left must be nullptr,
					// after the transplanting, successor has no child.
					// now we transplant node.right to successor.right
					// and transplant node.left to successor.left
					transplant(successor->right<Container<T>>(), node->right<Container<T>>(), successor, false);
					transplant(successor->left<Container<T>>(), node->left<Container<T>>(), successor, true);

					// now successor will take node's place
					transplant(node, successor, node->get_parent<Container<T>>(), b_node_left_right);
				}
				else // the successor is node->right
				{
					// we plant node->left to successor's
					transplant(successor->left<Container<T>>(), node->left<Container<T>>(), successor, true);

					// and now successor will take node's place
					transplant(node, successor, node->get_parent<Container<T>>(), b_node_left_right);
				}
			}

			return res;
		}
	};

}
