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
        TestObject* p = pool.alloc(42, "benchmark_object");
        benchmark::DoNotOptimize(p);
        pool.free(p);
    }
}
BENCHMARK(BM_StaticMemoryPoolAllocFree);

static void BM_NewDelete(benchmark::State& state) {
    for (auto _ : state) {
        TestObject* p = new TestObject(42, "heap");
        benchmark::DoNotOptimize(p);
        delete p;
    }
}
BENCHMARK(BM_NewDelete);

BENCHMARK_MAIN();
