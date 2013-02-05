/**
 * Mutable Fibonacci Heap implementation
 * © 2013 Philips Healthcare
 * Author: Maurice Termeer
 */

#ifndef _FIBONACCI_HEAP_
#define _FIBONACCI_HEAP_

#include <algorithm>
#include <array>
#include <deque>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <utility>
#include <vector>

template<typename T>
class fibonacci_heap
{
public:
	struct node {
		T element;
		node* parent;
		std::list<node*> children;
	};

	typedef T value_type;
	typedef std::size_t size_type;
	typedef const T& const_reference;
	typedef T& reference;
	typedef const node* const_pointer;
	typedef node* pointer;

	struct iterator {
		const_reference operator*() const {
			return p_->element;
		}

		reference operator*() {
			return p_->element;
		}

		iterator(pointer p) : p_(p) { }

		pointer p_;
	};

	fibonacci_heap()
		: min_element_(roots_.end()), n_(0) {
		degrees_.fill(nullptr);
	}

	iterator top() const
	{
		return iterator(*min_element_);
	}

	void insert(const_reference element)
	{
		pointer new_node = allocator_.allocate(1);
		allocator_.construct(new_node);
		new_node->element = element;
		new_node->parent = nullptr;
		roots_.push_back(new_node);

		if (n_ == 0 || new_node->element < (*min_element_)->element
		) {
			min_element_ = --roots_.end();
		}

		if (degrees_[0] == nullptr) {
			degrees_[0] = new_node;
		}

		++n_;
	}

	void pop()
	{
		pointer old_node = *min_element_;
		roots_.erase(min_element_);
		min_element_ = roots_.end();

		std::size_t degree = old_node->children.size();
		if (degrees_[degree] == old_node) {
			degrees_[degree] = nullptr;
		}

		std::for_each(
			std::begin(old_node->children), std::end(old_node->children),
				[&](pointer p) {
			p->parent = nullptr;
		});

		roots_.splice(roots_.end(), old_node->children);

		allocator_.deallocate(old_node, 1);

		merge_roots();

		--n_;
	}

private:
	void merge_roots()
	{
		auto i = roots_.begin();

		while (i != roots_.end()) {
			auto root = *i++;

			if (root->parent != nullptr) continue;

			auto degree = root->children.size();

			while (true) {
				auto k = degrees_[degree];

				if (k == nullptr) {
					degrees_[degree] = root;
					break;
				} else if (k == root) {
					break;
				} else {
					auto other = k;

					if (other->element < root->element) {
						other->children.push_back(root);
						root->parent = other;
						root = other;
					} else {
						root->children.push_back(other);
						other->parent = root;
					}

					degrees_[degree] = nullptr;
					++degree;
				}
			}
		}

		roots_.clear();

		for (auto j = std::begin(degrees_); j != std::end(degrees_); ++j) {
			if (*j == nullptr) continue;
			roots_.push_back(*j);

			if (roots_.size() == 1 ||
				(*j)->element < (*min_element_)->element
			) {
				min_element_ = --roots_.end();
			}
		}
	}

	std::array<pointer,sizeof(std::size_t) * 8> degrees_;
	std::list<pointer> roots_;
	typename std::list<pointer>::iterator min_element_;
	std::allocator<node> allocator_;
	size_type n_;
};

#endif // _FIBONACCI_HEAP_
