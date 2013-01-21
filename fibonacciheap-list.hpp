/**
 * Mutable Fibonacci Heap implementation
 * © 2013 Philips Healthcare
 * Author: Maurice Termeer
 */

#ifndef _FIBONACCI_HEAP_
#define _FIBONACCI_HEAP_

#include <algorithm>
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
		: degrees_(1), n_(0) { }

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

		if (roots_.empty() || new_node->element < roots_.front()->element) {
			roots_.push_front(new_node);

			if (degrees_[0] == typename std::list<pointer>::iterator()) {
				degrees_[0] = roots_.begin();
			}
		} else {
			roots_.push_back(new_node);

			if (degrees_[0] == typename std::list<pointer>::iterator()) {
				degrees_[0] = --roots_.end();
			}
		}

		++n_;
	}

	void pop()
	{
		pointer old_node = roots_.front();
		std::size_t degree = old_node->children.size();
		auto k = degrees_[degree];

		if (k == roots_.begin()) {
			degrees_[degree] = typename std::list<pointer>::iterator();
		}

		roots_.pop_front();

		std::for_each(
			std::begin(old_node->children), std::end(old_node->children),
				[&](pointer p) {

			if (roots_.empty() || p->element < roots_.front()->element) {
				roots_.push_front(p);
			} else {
				roots_.push_back(p);
			}

			p->parent = nullptr;
		});

		allocator_.deallocate(old_node, 1);

		merge_roots();

		--n_;
	}

private:
	void merge_roots()
	{
		std::vector<typename std::list<pointer>::iterator> to_erase;

		for (auto i = std::begin(roots_); i != std::end(roots_); ++i) {
			if ((*i)->parent != nullptr) continue;

			auto root = i;
			auto degree = (*root)->children.size();

			while (true) {
				if (degrees_.size() <= degree) {
					degrees_.resize(degree + 1);
				}

				auto k = degrees_[degree];

				if (k == typename std::list<pointer>::iterator()) {
					degrees_[degree] = root;
					break;
				} else if (k == root) {
					break;
				} else {
					auto other = k;

					if ((*other)->element < (*root)->element) {
						(*other)->children.push_back(*root);
						(*root)->parent = *other;
						to_erase.push_back(root);
						root = other;
					} else {
						(*root)->children.push_back(*other);
						(*other)->parent = *root;
						to_erase.push_back(other);
					}

					degrees_[degree] = typename std::list<pointer>::iterator();
					++degree;
				}
			}
		}

		using namespace std::placeholders;
		std::for_each(std::begin(to_erase), std::end(to_erase),
				[&](const typename std::list<pointer>::iterator i) {
			roots_.erase(i);
		});
	}

	std::vector<typename std::list<pointer>::iterator> degrees_;
	std::list<pointer> roots_;
	std::allocator<node> allocator_;
	size_type n_;
};

#endif // _FIBONACCI_HEAP_
