#pragma once
#include <atomic>
#include <array>
#include <concepts>
#include <expected>

template <size_t N>
concept NonZero = (N > 0);

template<typename T, size_t N>
requires NonZero<N>
class LockFreeQueue {
public:
    explicit LockFreeQueue() : _head(0), _tail(0) {
    }
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;
    LockFreeQueue(LockFreeQueue&&) = delete;
    LockFreeQueue& operator=(LockFreeQueue&&) = delete;
    ~LockFreeQueue() = default;


    std::expected<bool, std::string> enqueue(const T& item) {
        size_t tail = _tail.load(std::memory_order_relaxed);
        size_t nextTail = (tail + 1) % N;

        if (nextTail == _head.load(std::memory_order_acquire)) {
            return std::unexpected("LockFreeQueue: Queue is full");
        }

        _buffer[tail] = item;
        _tail.store(nextTail, std::memory_order_release);
        return true;
    }

    std::expected<bool, std::string> dequeue(T& item) {
        size_t head = _head.load(std::memory_order_relaxed);

        if (head == _tail.load(std::memory_order_acquire)) {
            return std::unexpected("LockFreeQueue: Queue is empty");
        }

        item = _buffer[head];
        _head.store((head + 1) % N, std::memory_order_release);
        return true;
    }

private:
    std::atomic<size_t> _head;
    std::atomic<size_t> _tail;
    std::array<T, N> _buffer;
};