/**
 * Mutable Fibonacci Heap implementation
 * © 2013 Philips Healthcare
 * Author: Maurice Termeer
 */

#ifndef _FIBONACCI_HEAP_
#define _FIBONACCI_HEAP_

#include <algorithm>
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
		std::vector<node*> children;
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
		: n_(0) { }

	iterator top() const
	{
		return iterator(roots_.front());
	}

	void insert(const_reference element)
	{
		pointer new_node = allocator_.allocate(1);
		allocator_.construct(new_node);
		new_node->element = element;
		new_node->parent = nullptr;
		roots_.push_back(new_node);

		if (new_node->element < roots_.front()->element) {
			std::swap(roots_.front(), roots_.back());
		}

		++n_;
	}

	void pop()
	{
		pointer old_node = roots_.front();

		std::swap(roots_.front(), roots_.back());
		roots_.pop_back();

		std::for_each(
			std::begin(old_node->children), std::end(old_node->children),
				[&](pointer p) {
			roots_.push_back(p);
			p->parent = nullptr;
		});

		allocator_.deallocate(old_node, 1);

		merge_roots();

		--n_;
	}

private:
	void merge_roots()
	{
		std::vector<pointer> degrees;

		for (std::size_t i = 0; i < roots_.size(); ++i) {
			auto root = roots_[i];

			if (root->parent != nullptr) continue;

			auto degree = root->children.size();

			while (true) {
				if (degrees.size() <= degree) {
					degrees.resize(degree + 1);
				}

				auto k = degrees[degree];

				if (k == nullptr) {
					degrees[degree] = root;
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

					degrees[degree] = nullptr;
					++degree;
				}
			}
		}

		roots_.clear();

		for (auto i = std::begin(degrees); i != std::end(degrees); ++i) {
			if (*i == nullptr) continue;
			roots_.push_back(*i);

			if (!(roots_.front()->element < (*i)->element)) {
				std::swap(roots_.front(), roots_.back());
			}
		}
	}

	std::deque<pointer> roots_;
	std::allocator<node> allocator_;
	size_type n_;
};

#endif // _FIBONACCI_HEAP_
