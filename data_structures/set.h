#pragma once

#include "avl_tree.h"

namespace ghl
{
	template <typename T>
	class set
	{
	public:
		set() {}
		virtual ~set() {}

		/*
		* Adds ele to the set, which takes its ownership
		* @returns true iff ele is successfully added
		*/
		virtual bool add(T* ele) = 0;
		/*
		* Emplaces an element with args to the set, which takes its ownership
		* @returns true iff the element is successfully added
		*/
		template <typename ... Args>
		bool emplace(Args&&... args)
		{
			return remove(new T(std::forward(args)...));
		}

		// @returns true iff ele is in the set
		virtual bool contains(const T& ele) const = 0;
		/*
		* removes the element = ele in the set
		* @returns true iff the removal is successful
		*/
		virtual bool remove(const T& ele) = 0;
		/*
		* removes an arbitrary element the set but does not destroy it
		* @returns the pointer to the element is returned, or nullptr if the set is empty
		*/
		virtual T* any_element() = 0;

		// @returns true iff the set is empty
		virtual bool empty() const { return 0 == size(); }
		// @returns the cardinality of the set
		virtual size_t size() const = 0;
	};

	template <typename T>
	class tree_set : public set<T>
	{
	private:
		using node_t = typename avl_tree<T>::node_t;

	public:
		tree_set() : set<T>() {}
		~tree_set() {}

	public:
		bool add(T* ele) override 
		{
			if (container.insert(ele, false).valid()) 
			{ 
				++num_eles; 
				return true; 
			}
			else
			{
				return false;
			}
		}

		bool contains(const T& ele) const override { return container.find(ele).valid(); }
		
		bool remove(const T& ele) override 
		{
			if (container.remove(ele))
			{
				--num_eles;
				return true;
			}
			else
			{
				return false;
			}
		}

		T* any_element() override 
		{
			/*
			* This implementation gives the leftmost element in the tree
			*/
			T* res = nullptr;

			// y will be the leftmost node
			node_t* y = container.get_root();

			if (y != nullptr)
			{
				node_t* x = y->left<node_t>();

				while (x != nullptr)
				{
					y = x;
					x = y->left<node_t>();
				}

				if (y != container.get_root()) // y is not root
				{
					res = y->release_object();
					y->get_parent<node_t>()->reset_left();
				}
				else // y is root
				{
					res = y->release_object();
					node_t* r = y->release_right();
					container.set_root(r);
				}
			}
			
			if (res != nullptr) --num_eles;

			return res;
		}

		size_t size() const override { return num_eles; }

	private:
		avl_tree<T> container;

		size_t num_eles = 0;
	};
}