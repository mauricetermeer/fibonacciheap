#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <queue>
#include <random>
#include <vector>

#include "fibonacciheap-ptr.hpp"
#include "boost/heap/fibonacci_heap.hpp"

int main()
{
	std::uniform_int_distribution<> distribution;
	auto generator = std::bind(distribution, std::mt19937());

	//const int k = 50, k2 = k;
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

	std::cout << "---\n";

	fibonacci_heap<int> fheap;
	std::array<fibonacci_heap<int>::iterator,10> fheap_it;

	for (int i = 0; i < 10; ++i) {
		fheap_it[i] = heap.insert(2 * i);
	}

	for (int i = 0; i < 5; ++i) {
		std::cout << *heap.top() << std::endl;
		heap.pop();
	}

	*fheap_it[7] = 11;
	fheap.decrease_key(fheap_it[7]);

	std::cout << "--\n";

	for (int i = 0; i < 5; ++i) {
		std::cout << *heap.top() << std::endl;
		heap.pop();
	}

	return 0;
}
