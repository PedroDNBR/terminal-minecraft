#pragma once

#include <atomic>
#include <utility>

template<typename T>
class MPSCQueue
{
	struct Node
	{
		T value{};
		std::atomic<Node*> next{ nullptr };
	};

	alignas(64) std::atomic<Node*> head_;
	alignas(64) std::atomic<Node*> tail_;

public:
	MPSCQueue()
	{
		Node* stub = new Node();
		head_.store(stub, std::memory_order_relaxed);
		tail_ = stub;
	}

	~MPSCQueue()
	{
		T dummy;
		while (pop(dummy)) {}
		delete tail_;
	}

	void push(T value)
	{
		Node* node = new Node();
		node->value = std::move(value);
		Node* previous = head_.exchange(node, std::memory_order_acq_rel);
		previous->next.store(node, std::memory_order_release);
	}

	bool pop(T& out)
	{
		Node* tail = tail_;
		Node* next = tail->next.load(std::memory_order_acquire);
		if (!next) return false;
		out = std::move(next->value);
		tail_ = next;
		delete tail;
		return true;
	}
};