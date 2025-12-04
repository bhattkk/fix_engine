#pragma once
#include <cstddef>
#include <new>
#include <type_traits>
#include <stdexcept>
#include "Macros.h"
#include <concepts>

template <size_t N>
concept Multipleof64 = ((N % 64) == 0 && N > 0);

template<typename T, size_t N>
requires Multipleof64<N>
class StaticMemoryPool {
    // Ensure that the beginning of this buffer is aligned to the alignment requirement of T.
    // Why this matters:
    //   - Objects of type T may require their starting address to be a multiple of alignof(T).
    //   - std::byte has an alignment of 1, so without alignas(...) the buffer could start at
    //     an address that is NOT suitable for constructing a T using placement new.
    //   - If the first address of the buffer is correctly aligned, then every subsequent
    //     object placed inside the buffer will also be correctly aligned, because sizeof(T)
    //     is guaranteed by the C++ standard to be a multiple of alignof(T).
    //   - Misaligned construction of T results in undefined behavior, potential crashes,
    //     or performance penalties depending on the platform.
    //   - alignas(alignof(T)) enforces this alignment at compile time with zero runtime cost.
    // In summary: This ensures the buffer can safely store N objects of type T using
    // placement new, with each object meeting T's alignment requirements.
    alignas(alignof(T)) std::byte _buffer[N * sizeof(T)];

    static constexpr size_t CHUNK_SIZE = 64;

    static constexpr size_t NUM_CHUNKS = N / CHUNK_SIZE;
    uint64_t bitmasks[NUM_CHUNKS] = {0};
    int _freeIndex = 0;

public:
    StaticMemoryPool() = default;
    StaticMemoryPool(const StaticMemoryPool&) = delete;
    StaticMemoryPool& operator=(const StaticMemoryPool&) = delete;
    StaticMemoryPool(StaticMemoryPool&&) = delete;
    StaticMemoryPool& operator=(StaticMemoryPool&&) = delete;

    ~StaticMemoryPool() {
        // Call destructors for all allocated objects
        // for (size_t i = 0; i < N; ++i) {
        //     if (_allocated[i]) {
        //         T* obj = reinterpret_cast<T*>(_buffer + i * sizeof(T));
        //         obj->~T();
        //     }
        // }
    }

    template<typename... Args>
    T* alloc(Args&&... args) {

        if (UNLIKELY(_freeIndex == -1)) {
            throw std::runtime_error("StaticMemoryPool: No free memory available for allocation");
        }

        void* place = _buffer + _freeIndex * sizeof(T);
        T* obj = new(place) T(std::forward<Args>(args)...);

        // Mark the current slot as allocated
        int chunkIndex = _freeIndex / CHUNK_SIZE;
        int bitIndex = _freeIndex % CHUNK_SIZE;
        bitmasks[chunkIndex] |= (1ULL << bitIndex);

        _freeIndex = -1; // This indicates all slots are full now
        // find next free index using __builtin_ffsl
        for (size_t i = 0; i < NUM_CHUNKS; ++i) {
            int bitPos = __builtin_ffsll(~bitmasks[i]);
            if (bitPos != 0) {
                _freeIndex = i * CHUNK_SIZE + (bitPos - 1);
                break;
            }
        }
        
        return obj;
    }

    void free(T* ptr) {
        if (UNLIKELY(ptr == nullptr)) {
            throw std::invalid_argument("StaticMemoryPool: Cannot deallocate a null pointer");
        }

        size_t index = (reinterpret_cast<std::byte*>(ptr) - _buffer) / sizeof(T);
        if (UNLIKELY(index >= N)) {
            throw std::out_of_range("StaticMemoryPool: Pointer out of range for deallocation");
        }

        // Call the destructor
        ptr->~T();

        // Mark the slot as free
        int chunkIndex = index / CHUNK_SIZE;
        int bitIndex = index % CHUNK_SIZE;
        bitmasks[chunkIndex] &= ~(1ULL << bitIndex);

        // Update _freeIndex if necessary i.e. deallocated index was the lastest free index
        if (UNLIKELY(_freeIndex == -1)) {
            _freeIndex = index;
        }
    }
};
