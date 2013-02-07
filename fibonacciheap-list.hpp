/**
 * Mutable Fibonacci Heap implementation
 * © 2013 Philips Healthcare
 * Author: Maurice Termeer
 */

#ifndef _FIBONACCI_HEAP_
#define _FIBONACCI_HEAP_

#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <list>
#include <utility>

template<typename T>
class fibonacci_heap
{
public:
	struct node {
		T element;
		bool marked;
		node* parent;
		std::list<node*> children;
	};

	typedef T value_type;
	typedef std::size_t size_type;
	typedef const T& const_reference;
	typedef T& reference;
	typedef const node* const_pointer;
	typedef node* pointer;
	typedef typename std::list<pointer>::iterator root_pointer;

	struct iterator {
		const_reference operator*() const {
			return p_->element;
		}

		reference operator*() {
			return p_->element;
		}

		iterator() : p_(nullptr) { }

		iterator(pointer p) : p_(p) { }

		pointer p_;
	};

	fibonacci_heap()
		: min_element_(roots_.end()), n_(0) {
	}

	iterator top() const
	{
		return iterator(*min_element_);
	}

	iterator insert(const_reference element)
	{
		pointer new_node = allocator_.allocate(1);
		allocator_.construct(new_node);
		new_node->element = element;
		new_node->marked = false;
		new_node->parent = nullptr;
		roots_.push_back(new_node);

		if (n_ == 0 || new_node->element < (*min_element_)->element
		) {
			min_element_ = --roots_.end();
		}

		++n_;

		return iterator(new_node);
	}

	void pop()
	{
		pointer old_node = *min_element_;

		roots_.erase(min_element_);
		min_element_ = roots_.end();

		std::for_each(
			std::begin(old_node->children), std::end(old_node->children),
				[&](pointer p) {
			p->marked = false;
			p->parent = nullptr;
		});

		roots_.splice(roots_.end(), old_node->children);

		allocator_.deallocate(old_node, 1);

		merge_roots();

		--n_;
	}

	void decrease_key(iterator it)
	{
		pointer p = it.p_;
		pointer parent = p->parent;

		if (parent == nullptr) return;
		if (!(p->element < parent->element)) return;

		for (;;) {
			auto k = std::find(
				parent->children.begin(),
				parent->children.end(),
				p);
			roots_.splice(roots_.end(), parent->children, k);
			p->marked = false;

			p = parent;
			parent = p->parent;

			if (parent == nullptr) break;

			if (!p->marked) {
				p->marked = true;
				break;
			}
		}
	}

private:
	void merge_roots()
	{
		auto invalid = roots_.end();
		min_element_ = invalid;
		degrees_.fill(invalid);

		auto i = roots_.begin();

		while (i != roots_.end()) {
			auto root = i++;

			if ((*root)->parent != nullptr) continue;

			auto degree = (*root)->children.size();

			while (true) {
				auto k = degrees_[degree];

				if (k == invalid) {
					degrees_[degree] = root;
					break;
				} else {
					auto other = k;

					if ((*other)->element < (*root)->element) {
						(*root)->parent = *other;
						(*other)->children.splice(
							(*other)->children.end(), roots_, root);
						root = other;
					} else {
						(*other)->parent = *root;
						(*root)->children.splice(
							(*root)->children.end(), roots_, other);
					}

					degrees_[degree++] = invalid;
				}
			}

			if (min_element_ == invalid ||
				(*root)->element < (*min_element_)->element
			) {
				min_element_ = root;
			}
		}
	}

	std::array<root_pointer, sizeof(std::size_t) * 8> degrees_;
	std::list<pointer> roots_;
	root_pointer min_element_;
	std::allocator<node> allocator_;
	size_type n_;
};

#endif // _FIBONACCI_HEAP_
