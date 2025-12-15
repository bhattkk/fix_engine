#pragma once
#include <cstddef>
#include <cstdlib>
#include <new>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <algorithm>

template<typename T>
class HeapMemoryPool {
    std::byte* _buffer = nullptr;
    size_t _capacity = 0;
    std::vector<bool> _allocated;

public:
    explicit HeapMemoryPool(size_t initialCapacity)
        : _capacity(initialCapacity), _allocated(initialCapacity, false) 
    {
        _buffer = static_cast<std::byte*>(std::aligned_alloc(alignof(T), sizeof(T) * _capacity));
        if (!_buffer) throw std::bad_alloc();
    }

    ~HeapMemoryPool() {
        // Call destructors for allocated objects
        for (size_t i = 0; i < _capacity; ++i) {
            if (_allocated[i]) {
                T* obj = reinterpret_cast<T*>(_buffer + i * sizeof(T));
                obj->~T();
            }
        }
        std::free(_buffer);
    }

    template<typename... Args>
    T* allocate(Args&&... args) {
        for (size_t i = 0; i < _capacity; ++i) {
            if (!_allocated[i]) {
                void* place = _buffer + i * sizeof(T);
                T* obj = new(place) T(std::forward<Args>(args)...);
                _allocated[i] = true;
                return obj;
            }
        }
        // Optional: grow pool dynamically
        grow();
        return allocate(std::forward<Args>(args)...);
    }

    void deallocate(T* ptr) {
        size_t idx = (reinterpret_cast<std::byte*>(ptr) - _buffer) / sizeof(T);
        if (idx >= _capacity || !_allocated[idx])
            throw std::runtime_error("Invalid pointer deallocation");
        ptr->~T();
        _allocated[idx] = false;
    }

private:
    void grow() {
        size_t newCapacity = _capacity * 2;
        std::byte* newBuffer = static_cast<std::byte*>(std::aligned_alloc(alignof(T), sizeof(T) * newCapacity));
        if (!newBuffer) throw std::bad_alloc();

        // Move existing objects to new buffer
        for (size_t i = 0; i < _capacity; ++i) {
            if (_allocated[i]) {
                T* oldObj = reinterpret_cast<T*>(_buffer + i * sizeof(T));
                T* newObj = new(newBuffer + i * sizeof(T)) T(std::move(*oldObj));
                oldObj->~T();
            }
        }

        std::free(_buffer);
        _buffer = newBuffer;
        _allocated.resize(newCapacity, false);
        _capacity = newCapacity;
    }
};
