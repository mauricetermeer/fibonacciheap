/**
 * Mutable Fibonacci Heap example / test / benchmark
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

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <queue>
#include <random>
#include <vector>

#include "fibonacciheap.hpp"

//#define TEST_BOOST
#ifdef TEST_BOOST
#include "boost/heap/fibonacci_heap.hpp"
#endif // TEST_BOOST

int main()
{
	std::uniform_int_distribution<> distribution;
	auto generator = std::bind(distribution, std::mt19937());

	const int k = 3000000, k2 = k / 10;

	std::vector<int> pool(k);
	std::for_each(std::begin(pool), std::end(pool),
		[&](int& i) { i = generator(); });

	std::priority_queue<int> queue;

	auto t0 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k; ++i) {
		queue.push(pool[i]);
	}

	auto t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k2; ++i) {
		queue.pop();
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto diff0 = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	auto diff1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "priority_queue: " << diff0 << " / " << diff1 << std::endl;

	fibonacci_heap<int> heap;

	t0 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k; ++i) {
		heap.insert(pool[i]);
	}

	t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k2; ++i) {
		heap.pop();
	}

	t2 = std::chrono::high_resolution_clock::now();
	diff0 = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	diff1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "fibonacci_heap: " << diff0 << " / " << diff1 << std::endl;

#ifdef TEST_BOOST
	boost::heap::fibonacci_heap<int> bheap;

	t0 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k; ++i) {
		bheap.push(pool[i]);
	}

	t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k2; ++i) {
		bheap.pop();
	}

	t2 = std::chrono::high_resolution_clock::now();
	diff0 = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	diff1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "boost::fibonacci_heap: " << diff0 << " / " << diff1 << std::endl;
#endif // TEST_BOOST

	fibonacci_heap<int> fheap;
	std::array<fibonacci_heap<int>::iterator,10> fheap_it;
	std::vector<int> output;

	for (int i = 0; i < 10; ++i) {
		fheap_it[i] = heap.insert(2 * i);
	}

	for (int i = 0; i < 5; ++i) {
		output.push_back(*heap.top());
		heap.pop();
	}

	*fheap_it[7] = 11;
	fheap.decrease_key(fheap_it[7]);

	for (int i = 0; i < 5; ++i) {
		output.push_back(*heap.top());
		heap.pop();
	}

	assert(is_sorted(std::begin(output), std::end(output)));

	return 0;
}
