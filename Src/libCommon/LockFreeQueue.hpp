#pragma once
#include <atomic>
#include <array>
#include <concepts>
#include <expected>


// Memory order explanations:
// https://stackoverflow.com/questions/12346487/what-do-each-memory-order-mean/70585811#70585811

template <size_t N>
concept NonZero = (N > 0);

template <size_t N>
concept PowerOfTwo = (N & (N - 1)) == 0;

template<size_t N>
concept ValidSize = NonZero<N> && PowerOfTwo<N>;

template <typename T>
concept ValidQueueElement =
    std::default_initializable<T> &&
    std::copyable<T> &&
    std::movable<T> &&
    std::assignable_from<T&, const T&> &&
    std::assignable_from<T&, T&&>; 

template<typename T, size_t N>
requires ValidSize<N> && ValidQueueElement<T>
class LockFreeQueue {
public:
    LockFreeQueue() = default;
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;
    LockFreeQueue(LockFreeQueue&&) = delete;
    LockFreeQueue& operator=(LockFreeQueue&&) = delete;
    ~LockFreeQueue() = default;

    std::expected<bool, std::string> enqueue(const T& item) {
        size_t t = _tail.load(std::memory_order_relaxed);
        size_t h = _head.load(std::memory_order_acquire);

        if (((t + 1) % N) == h)
            return std::unexpected("LockFreeQueue: Queue is full");

        _buffer[t] = (item);                  // safe: only producer writes t

        _tail.store((t + 1) & (N - 1), std::memory_order_release);
        return true;
    }

    std::expected<bool, std::string> dequeue(T& item) {
        size_t h = _head.load(std::memory_order_relaxed);
        size_t t = _tail.load(std::memory_order_acquire);

        if (h == t)
            return std::unexpected("LockFreeQueue: Queue is empty");

        item = (_buffer[h]);                 // safe: only consumer reads h

        _head.store((h + 1) & (N - 1), std::memory_order_release);
        return true;
    }

private:
    std::atomic<size_t> _head = 0;
    std::atomic<size_t> _tail = 0;
    std::array<T, N> _buffer;
};