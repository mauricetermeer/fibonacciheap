#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <queue>
#include <random>
#include <vector>

#include "boost/heap/fibonacci_heap.hpp"
#include "fibonacciheap-list.hpp"

int main()
{
	std::uniform_int_distribution<> distribution;
	auto generator = std::bind(distribution, std::mt19937());

	const int k = 1000000;
	const int k2 = k / 10;

	std::vector<int> pool(k);
	std::for_each(std::begin(pool), std::end(pool),
		[&](int& i) { i = generator(); });

	fibonacci_heap<int> heap;

	auto t0 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k; ++i) {
		heap.insert(pool[i]);
	}

	auto t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k2; ++i) {
		heap.pop();
	}

	std::cout << std::endl;

	auto t2 = std::chrono::high_resolution_clock::now();
	auto diff0 = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	auto diff1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "fibonacci_heap: " << diff0 << " / " << diff1 << std::endl;

	std::priority_queue<int> queue;

	t0 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k; ++i) {
		queue.push(pool[i]);
	}

	t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < k2; ++i) {
		queue.pop();
	}

	t2 = std::chrono::high_resolution_clock::now();
	diff0 = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	diff1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "priority_queue: " << diff0 << " / " << diff1 << std::endl;

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

	return 0;
}
