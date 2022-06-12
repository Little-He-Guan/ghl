#pragma once

#include "tree.h"

namespace ghl
{

	/*
	* T must have a total ordering imposed by operator<=, and equality imposed by operator==
	* If T can be represented by light weight key, then T must also have operator<= and operator== that take a key
	*
	* Invariant: the binary search tree property.
	*/
	template <typename T>
	class binary_search_tree : public binary_tree<T>
	{
	public:
		binary_search_tree() {}
		/*
		* Assumes that the pointers own the elements
		* After it, the ownerships will be taken
		*/
		binary_search_tree(const std::initializer_list<T*>& init_list)
		{
			for (auto* p : init_list)
			{
				insert(p);
			}
		}

		virtual ~binary_search_tree() {}

	public:
		friend struct iterator;

		struct iterator
		{
			iterator() {}
			explicit iterator(binary_node<T>* n) : node(n) {}
			iterator(const iterator& o) : node(o.node) {}
			iterator(iterator&& o) : node(o.node) {}
			~iterator() {}

			iterator& operator=(const iterator& r) { node = r.node; return *this; }
			iterator& operator=(iterator&& r) { node = r.node; return *this; }

			bool valid() const { return node != nullptr ? (!node->empty()) : false; }

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

					binary_node<T>* child, *parent = node;
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
					return iterator(binary_search_tree::internal_maximum(node->left()));
				}
				else
				{
					// if its children have no value smaller than the val it has,
					// then a smaller value can only be held by some of its parent who's bigger than the branch contains it
					// (i.e. it or its some parent is a right child of a node)

					binary_node<T>* child, *parent = node;
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
			binary_node<T>* x = this->root.get();
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
				this->root.reset(res = new binary_node<T>(nullptr, ele));
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
		* 
		* @returns true iff pos is valid and is removed
		*/
		bool remove(iterator pos)
		{
			if (pos.valid())
			{
				binary_node<T>* node = pos.node;

				// tells if node is its parent's left or right, used in transparent
				// if node->p is nullptr, then it will be ignored by transplant, and we simply set it to ture then.
				bool b_node_left_right = node->p != nullptr ? node == node->p->left() : true;

				/*
				* 1. node has no children. replace it with nullptr in its parent
				* 2. node has only 1 child. replace it with its only child in its parent.
				* 3. node has 2 children. replace it with its successor, and rearrange the tree.
				*/

				if (node->left() == nullptr) // if left is empty, then whether right is empty or not, we can replace node with its right anyway
				{
					transplant(node, node->right(), node->p, b_node_left_right);
				}
				else if (node->right() == nullptr) // right is empty
				{
					transplant(node, node->left(), node->p, b_node_left_right);
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
						transplant(ps->left(), successor->right(), ps, true);

						// because successor's left must be nullptr,
						// after the transplanting, successor has no child.
						// now we transplant node.right to successor.right
						// and transplant node.left to successor.left
						transplant(successor->right(), node->right(), successor, false);
						transplant(successor->left(), node->left(), successor, true);

						// now successor will take node's place
						transplant(node, successor, node->p, b_node_left_right);
					}
					else // the successor is node->right
					{
						// we plant node->left to successor's
						transplant(successor->left(), node->left(), successor, true);

						// and now successor will take node's place
						transplant(node, successor, node->p, b_node_left_right);
					}
				}

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
		iterator find(const T& ele) const { return iterator(internal_find(this->root.get(), ele)); }
		/*
		* @returns the iterator to the element represented by k if it's present, or otherwise an invalid iter
		*/
		template <typename Key>
		iterator find(Key k) const { return iterator(internal_find(this->root.get(), k)); }

		iterator maximum() const { return iterator(internal_maximum(this->root.get())); }
		iterator minimum() const { return iterator(internal_minimum(this->root.get())); }

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

			return node;
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

			return node;
		}

		static binary_node<T>* internal_maximum(binary_node<T>* node)
		{
			while (node->right() != nullptr)
			{
				node = node->right();
			}

			return node;
		}
		static binary_node<T>* internal_minimum(binary_node<T>* node)
		{
			while (node->left() != nullptr)
			{
				node = node->left();
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
		void transplant(binary_node<T>* u, binary_node<T>* v, binary_node<T>* up, bool b_u_left_right)
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
	};

}
