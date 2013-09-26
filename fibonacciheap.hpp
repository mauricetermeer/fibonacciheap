/**
 * Mutable Fibonacci Heap implementation
 * Version 11 (2013-09-26)
 *
 * Copyright © 2013, Maurice Termeer
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _FIBONACCI_HEAP_
#define _FIBONACCI_HEAP_

#include <array>

template<typename T>
class fibonacci_heap
{
public:
	struct node {
		T element;
		bool marked;
		node* parent;
		node *first, *last;
		std::size_t degree;
		node *next, *prev;
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

		bool operator==(const std::nullptr_t q) const {
			return p_ == q;
		}

		bool operator!=(const std::nullptr_t q) const {
			return p_ != q;
		}

		iterator() : p_(nullptr) { }

		iterator(std::nullptr_t p) : p_(p) { }

		iterator(pointer p) : p_(p) { }

		pointer p_;
	};

	fibonacci_heap()
		: first_(nullptr), last_(nullptr), min_element_(nullptr), n_(0) {
	}

	~fibonacci_heap()
	{
		for (pointer p = first_; p != nullptr;) {
			pointer q = p;
			p = p->next;
			free_node(q);
		}
	}

	std::size_t size() const {
		return n_;
	}

	iterator top() const
	{
		return iterator(min_element_);
	}

	iterator insert(const_reference element)
	{
		pointer new_node = new node();
		new_node->element = element;
		new_node->marked = false;
		new_node->parent = nullptr;
		new_node->degree = 0;
		new_node->first = nullptr;
		new_node->last = nullptr;

		if (first_ == nullptr) {
			new_node->prev = nullptr;
			new_node->next = nullptr;
			first_ = new_node;
			last_ = new_node;
		} else {
			new_node->prev = last_;
			new_node->next = nullptr;
			last_->next = new_node;
			last_ = new_node;
		}

		if (n_ == 0 || new_node->element < min_element_->element
		) {
			min_element_ = new_node;
		}

		++n_;

		return iterator(new_node);
	}

	void pop()
	{
		if (min_element_->prev != nullptr) {
			min_element_->prev->next = min_element_->next;
		}

		if (min_element_->next != nullptr) {
			min_element_->next->prev = min_element_->prev;
		}

		if (first_ == min_element_) {
			first_ = min_element_->next;
		}

		if (last_ == min_element_) {
			last_ = min_element_->prev;
		}

		for (pointer p = min_element_->first; p != nullptr;) {
			pointer q = p;
			p = p->next;

			q->marked = false;
			q->parent = nullptr;

			if (first_ == nullptr) {
				q->prev = nullptr;
				q->next = nullptr;
				first_ = q;
				last_ = q;
			} else {
				q->prev = last_;
				q->next = nullptr;
				last_->next = q;
				last_ = q;
			}
		}

		delete min_element_;
		min_element_ = nullptr;

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
			if (p->prev != nullptr) {
				p->prev->next = p->next;
			}

			if (p->next != nullptr) {
				p->next->prev = p->prev;
			}

			if (parent->first == p) {
				parent->first = p->next;
			}

			if (parent->last == p) {
				parent->last = p->prev;
			}

			--parent->degree;

			if (last_ == nullptr) {
				p->prev = nullptr;
				p->next = nullptr;
				first_ = last_ = p;
			} else {
				p->prev = last_;
				p->next = nullptr;
				last_->next = p;
				last_ = p;
			}

			p->parent = nullptr;
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
		min_element_ = nullptr;
		degrees_.fill(nullptr);

		auto i = first_;

		while (i != nullptr) {
			auto root = i;
			i = i->next;

			auto degree = root->degree;

			while (true) {
				auto k = degrees_[degree];

				if (k == nullptr) {
					degrees_[degree] = root;
					break;
				} else {
					auto other = k;

					if (other->element < root->element) {
						if (root->prev != nullptr) {
							root->prev->next = root->next;
						}

						if (root->next != nullptr) {
							root->next->prev = root->prev;
						}

						if (root->parent != nullptr) {
							if (root->parent->first == root) {
								root->parent->first = root->next;
							}

							if (root->parent->last == root) {
								root->parent->last = root->prev;
							}

							--root->parent->degree;
						} else {
							if (first_ == root) {
								first_ = root->next;
							}

							if (last_ == root) {
								last_ = root->prev;
							}
						}

						root->parent = other;

						if (other->first == nullptr) {
							other->first = root;
							other->last = root;
							root->next = nullptr;
							root->prev = nullptr;
						} else {
							root->prev = other->last;
							root->next = nullptr;
							other->last->next = root;
							other->last = root;
						}

						++other->degree;
						root = other;
					} else {
						if (other->prev != nullptr) {
							other->prev->next = other->next;
						}

						if (other->next != nullptr) {
							other->next->prev = other->prev;
						}

						if (other->parent != nullptr) {
							if (other->parent->first == other) {
								other->parent->first = other->next;
							}

							if (other->parent->last == other) {
								other->parent->last = other->prev;
							}

							--other->parent->degree;
						} else {
							if (first_ == other) {
								first_ = other->next;
							}

							if (last_ == other) {
								last_ = other->prev;
							}
						}

						other->parent = root;

						if (root->first == nullptr) {
							root->first = other;
							root->last = other;
							other->next = nullptr;
							other->prev = nullptr;
						} else {
							other->prev = root->last;
							other->next = nullptr;
							root->last->next = other;
							root->last = other;
						}

						++other->parent->degree;
					}

					degrees_[degree] = nullptr;
					++degree;
				}
			}

			if (min_element_ == nullptr ||
				min_element_->parent != nullptr ||
				root->element < min_element_->element
			) {
				min_element_ = root;
			}
		}
	}

	void free_node(pointer n)
	{
		for (pointer p = n->first; p != nullptr;) {
			pointer q = p;
			p = p->next;
			free_node(q);
		}

		delete n;
	}

	std::array<pointer, sizeof(std::size_t) * 8> degrees_;
	pointer first_, last_;
	pointer min_element_;
	size_type n_;
};

#endif // _FIBONACCI_HEAP_
