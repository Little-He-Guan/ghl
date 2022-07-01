#pragma once

#include "binary_search_tree.h"

namespace ghl
{
	enum class avl_tree_imbalance_type : char
	{
		/*
		* 1. An insertion into the left subtree of the left child of y.
		* 2. An insertion into the right subtree of the left child of y.
		* 3. An insertion into the left subtree of the right child of y.
		* 4. An insertion into the right subtree of the right child of y.
		*/
		LL,
		RL,
		LR,
		RR
	};

	/*
	* AVL tree implemented by maintaining height of nodes and rotating the tree when it's imbalanced
	*/
	template <typename T>
	class avl_tree final : public binary_search_tree<T, binary_tree_with_height> // height is used
	{
		// used for unit tests
		friend class avl_tree_tester;
	private:
		using super = binary_search_tree<T, binary_tree_with_height>;
	public:
		using iterator = typename super::iterator;
		using node_t = typename super::node_t;

	public:
		avl_tree() : super() {}
		explicit avl_tree(node_t* rt) : super(rt) {}
		avl_tree(const std::initializer_list<T*>& init_list) : super()
		{
			for (auto p : init_list)
			{
				insert(p);
			}
		}
		~avl_tree() {}

	private:
		struct imbalance_info
		{
			// the logical flow will be like,
			// first an info object is created with balanced set to true
			// then a balance check is performed.
			// If it is balanced, then the obj can be returned with no modification.
			// If it is not, then we set it to false and add more info and return it.
			imbalance_info() : balanced(true) {}
			imbalance_info(node_t* p, avl_tree_imbalance_type t) : pos(p), type(t), balanced(false) {}
			imbalance_info(const imbalance_info& other) : pos(other.pos), type(other.type), balanced(other.balanced) {}

			node_t* pos;
			avl_tree_imbalance_type type;
			bool balanced;
		};

	public:
		iterator insert(T * ele, bool bAllowDuplication = true) override
		{
			auto pos = super::insert(ele, bAllowDuplication);
			
			if (pos.valid())
			{
				auto info = check_balance_on_path(pos);
				if (!info.balanced) // the tree is imbalanced, rotate it
				{
					rotate(info);
				}
			}

			return pos;
		}

		bool remove(iterator pos) override
		{
			if (pos.valid())
			{
				node_t* n = this->internal_remove(pos.node);

				if (n)
				{
					// imbalance can only happen along this path after a removal
					auto info = check_balance_on_path(find_remove_path_end(iterator(n)));
					if (!info.balanced)
					{
						rotate(info);
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
		bool remove(const T& ele) { return this->remove(this->find(ele)); }
		/*
		* Removes the element if it is present
		*
		* @param k the key that represents the element
		* @returns true iff the element is removed
		*/
		template <typename Key>
		bool remove(Key k) { return this->remove(this->find(k)); }

	private:
		/*
		* Checks the balance on the path root->end
		* (if the balance is broken by an insert, then it will occur on the path from root to the pos where the ele is inserted)
		* 
		* @param end where the insertion is made, and where is supposed to be a leaf or the root
		*/
		imbalance_info check_balance_on_path(iterator end) const
		{
			// end can only be a leaf
			
			imbalance_info res{};

			node_t* x = end.node;
			node_t* y = x->get_parent<node_t>();

			while (y != nullptr) // until x is the root
			{
				size_t lh = y->has_left() ? y->left<node_t>()->get_height() : 0;
				size_t rh = y->has_right() ? y->right<node_t>()->get_height() : 0;

				size_t diff;
				if (lh >= rh) diff = lh - rh;
				else diff = rh - lh;

				if (diff > 1) // imbalanced
				{
					/*
					* now y is the node that needs to be rebalanced
					* x is its left/right child on whose subtrees the insertion is made
					* and recall that end is the pos where the insertion is made
					* 
					* We have four possible situations:
					* 1. An insertion into the left subtree of the left child of y.
					* 2. An insertion into the right subtree of the left child of y.
					* 3. An insertion into the left subtree of the right child of y.
					* 4. An insertion into the right subtree of the right child of y.
					*/

					res.balanced = false;
					res.pos = y;

					/*
					* if x is y->left(), then y has left
					* similarly, if x == y->right(), then y has right.
					* 
					* We don't need to check if y has left/right inside 
					*/
					if (x == y->left<node_t>()) // 1,2
					{
						res.type = y->left<node_t>()->is_on_left_or_right(end.node) ? avl_tree_imbalance_type::LL : avl_tree_imbalance_type::LR;
					}
					else // 3,4
					{
						res.type = y->right<node_t>()->is_on_left_or_right(end.node) ? avl_tree_imbalance_type::RL : avl_tree_imbalance_type::RR;
					}

					break;
				}
				else // balanced. continue.
				{
					x = y;
					y = x->get_parent<node_t>();
				}
			}

			return res;
		}

		/*
		* Deals with the imbalance according to the info by rotating the tree
		*/
		void rotate(const imbalance_info& info)
		{
			node_t* k1, * k2, * k3,
				/* * a, */ * b, * c, /* * d, */
				* p;
			auto update_parent_branch = [&p, this](node_t* child, node_t* new_child) -> void
			{
				if (p != nullptr) // child is not root
				{
					if (p->is_on_left_or_right(child))
					{
						p->release_left();
						p->set_left(new_child);
					}
					else
					{
						p->release_right();
						p->set_right(new_child);
					}
				}
				else // child is root
				{
					this->root.release();
					this->root.reset(new_child);
				}
			};

			switch (info.type)
			{
			case avl_tree_imbalance_type::LL:
				k2 = info.pos;
				p = k2->get_parent<node_t>();

				k1 = k2->release_left();
				//a = k1->left<node_t>();
				b = k1->release_right();

				update_parent_branch(k2, k1);

				k1->set_right(k2);
				k2->set_left(b);
				break;
			case avl_tree_imbalance_type::LR:
				k3 = info.pos;
				p = k3->get_parent<node_t>();

				k1 = k3->release_left();			
				k2 = k1->release_right();
				//a = k1->left<node_t>();
				b = k2->release_left();
				c = k2->release_right();

				update_parent_branch(k3, k2);

				k1->set_right(b);
				k3->set_left(c);

				k2->set_left(k1);
				k2->set_right(k3);
				break;
			case avl_tree_imbalance_type::RL:
				k1 = info.pos;
				p = k1->get_parent<node_t>();

				k3 = k1->release_right();
				k2 = k3->release_left();
				//a = k1->left<node_t>();
				b = k2->release_left();
				c = k2->release_right();

				update_parent_branch(k1, k2);

				k1->set_right(b);
				k3->set_left(c);

				k2->set_left(k1);
				k2->set_right(k3);
				break;
			case avl_tree_imbalance_type::RR:
				k1 = info.pos;
				p = k1->get_parent<node_t>();

				k2 = k1->release_right();
				//a = k1->left<node_t>();
				b = k2->release_left();

				update_parent_branch(k1, k2);

				k2->set_left(k1);
				k1->set_right(b);
				break;
			}
		}
	
		/*
		* As check_balance_on_path is designed to be called on the pos where an insertion is made,
		* we cannot simply call it on where the deletion has been made.
		* 
		* This function solves the problem by finding the pos the acts like where an insertion is made given the pos where the deletion is made
		* (Only call it when pos is valid)
		*/
		iterator find_remove_path_end(iterator pos)
		{
			node_t* y = pos.node;

			while (y->get_height() > 1) // until y does not have any children
			{
				if (y->has_left())
				{
					y = (y->left<node_t>()->get_height() == y->get_height() - 1) ? y->left<node_t>() : y->right<node_t>();
				}
				else
				{
					y = y->right<node_t>();
				}
			}

			// in the end y will be on a path that has the highest length
			// and y will never be nullptr
			return iterator(y);
		}
	};
}