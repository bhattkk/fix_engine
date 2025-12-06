#include <benchmark/benchmark.h>
#include "StaticMemoryPool.hpp"

namespace {
    class TestObject {
    public:
        explicit TestObject(int integer, std::string str) : _intParam(integer), _strParam(str) {}
        int intParam() const { return _intParam; }
        std::string strParam() const { return _strParam; }
        ~TestObject() = default;

    private:  
        int _intParam;
        std::string _strParam;
    };
}

static void BM_StaticMemoryPoolAllocFree(benchmark::State& state) {
    StaticMemoryPool<TestObject, 1024> pool;

    for (auto _ : state) {
        std::vector<TestObject*> pointers;
        for (int i=0; i < 1000; ++i) {
            TestObject* p = pool.alloc(42, "benchmark_object");
            pointers.push_back(p);
        }
        benchmark::DoNotOptimize(pointers);
        // delete odd numbers
        for (int i = 1; i < 1000; i += 2) {
            pool.free(pointers[i]);
            pointers[i] = pool.alloc(43, "realloc_object");
        }
        // delete even numbers
        for (int i = 0; i < 1000; i++) {
            pool.free(pointers[i]);
        }
    }
}
BENCHMARK(BM_StaticMemoryPoolAllocFree);

static void BM_NewDelete(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<TestObject*> pointers;
        for (int i=0; i < 1000; ++i) {
            TestObject* p = new TestObject(42, "benchmark_object");
            pointers.push_back(p);
        }
        benchmark::DoNotOptimize(pointers);
        // delete odd numbers
        for (int i = 1; i < 1000; i += 2) {
            delete pointers[i];
            pointers[i] = new TestObject(43, "realloc_object");
        }
        // delete even numbers
        for (int i = 0; i < 1000; i++) {
            delete pointers[i];
        }
    }
}
BENCHMARK(BM_NewDelete);

BENCHMARK_MAIN();
