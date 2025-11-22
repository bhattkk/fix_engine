#pragma once
#include <vector>

template<typename T>
class MemoryPool {
    std::vector<T> _pool;
    std::vector<bool> _allocated;
    size_t _poolSize = 0;
    size_t _currentSize = 0;
    size_t _freeIndex = 0;

public:
    MemoryPool(size_t initialSize = 0)
    {
        _poolSize = initialSize;
        _pool.resize(initialSize);
        _allocated.resize(initialSize, false);
    }

    ~MemoryPool()
    {
        // Destructor
    }

    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;

    template<typename... Args>
    T* allocate(Args&&... args)
    {
        // Allocate new object in the pool
        T* newObj = new(&_pool[_freeIndex]) T(std::forward<Args>(args)...);

        // Mark as allocated
        _allocated[_freeIndex] = true;

        // Update free index and current size
        // TODO : Condition is unlikely 
        if (_currentSize < _poolSize) {
            _currentSize++;
        } else {
            _poolSize = 2 * _poolSize;
            _pool.resize(_poolSize);
        }
        
        // Find next free index
        while (_allocated[_freeIndex]) {
            // TODO : Condition is unlikely
            if (_freeIndex + 1 == _pool.size()) {
                _freeIndex = 0;
            } else {
                _freeIndex++;
            }
        }
        
        
        return newObj;
    }

    void deallocate(T* obj)
    {
        size_t index = 0;
        for (auto it = _pool.begin(); it != _pool.end(); ++it) {
            if (&(*it) == obj) {
                _allocated[index] = false;
                _currentSize--;
                obj->~T(); // TODO: Call destructor. Is it required?
                break;
            }
            index++;
        }
    }

    /************ TODO: Needed for testing purposes. Should think of hiding these methods  **********/
    size_t CurrentSize() const {
        return _currentSize;
    }

    size_t PoolSize() const {
        return _poolSize;
    }
};