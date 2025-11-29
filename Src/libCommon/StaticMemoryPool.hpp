#pragma once
#include <cstddef>
#include <new>
#include <type_traits>
#include <stdexcept>

template<typename T, size_t N>
class StaticMemoryPool {
    static_assert(N > 0, "Pool size must be > 0");

    // Raw aligned memory on stack. 
    // No runtime overhead as alignment is checked at compile time. 
    alignas(alignof(T)) std::byte _buffer[N * sizeof(T)];
    bool _allocated[N]{false};

    int _freeIndex = 0;
    int _allocatedCount = 0;

public:
    StaticMemoryPool() = default;

    ~StaticMemoryPool() {
        // Call destructors for all allocated objects
        for (size_t i = 0; i < N; ++i) {
            if (_allocated[i]) {
                T* obj = reinterpret_cast<T*>(_buffer + i * sizeof(T));
                obj->~T();
            }
        }
    }

    template<typename... Args>
    T* allocate(Args&&... args) {

        // TODO: Very Unlikely scenario check
        if (_allocatedCount == N) {
            throw std::runtime_error("StaticMemoryPool: No free memory available for allocation");
        }
        void* place = _buffer + _freeIndex * sizeof(T);
        T* obj = new(place) T(std::forward<Args>(args)...);
        _allocated[_freeIndex] = true;
        _allocatedCount++;

        // TODO : Can we simply the logic here? Simplfy means should be performant too.
        // TODO : Very Unlikely scenario check
        if (_allocatedCount == N) {
            return obj; // No free slots left
        }
        // Find next free index
        do {
            _freeIndex++;

            if(_freeIndex == N)
            {
                _freeIndex = 0;
            }
        } while (_allocated[_freeIndex]);
        return obj;
    }

    void deallocate(T* ptr) {
        _allocatedCount--;
        size_t idx = (reinterpret_cast<std::byte*>(ptr) - _buffer) / sizeof(T);
        ptr->~T();
        _allocated[idx] = false;
    }
};
