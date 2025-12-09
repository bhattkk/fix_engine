#include <benchmark/benchmark.h>
#include <iostream>
#include "LockFreeQueue.hpp"

namespace {
    class ComplexStruct {
        long long a;
        double b;
        std::string c1;
        std::string c2;
    public:
        ComplexStruct() = default;
        ComplexStruct(long long val) 
        :a(val),
        b(val * 2.0),
        c1("Test" + std::to_string(val)),
        c2("Data" + std::to_string(val))
        {}

        void print() const {
            std::cout << "ComplexStruct(a=" << a << ", b=" << b 
                      << ", c1=" << c1 << ", c2=" << c2 << ")\n";
        }

        ~ComplexStruct() = default;
        // Making sure no implicit copy/move operations.
        ComplexStruct(const ComplexStruct&) = default;
        ComplexStruct& operator=(const ComplexStruct&) = default;
        ComplexStruct(ComplexStruct&&) = default;
        ComplexStruct& operator=(ComplexStruct&&) = default;
    };
} 

static void BM_EnqueueDequeue(benchmark::State& state) {
    static LockFreeQueue<ComplexStruct, 4096> q;
    long long local_i = 0;
    for (auto _ : state) {
        if (state.thread_index() == 0) {
            // Producer
            benchmark::DoNotOptimize(q.enqueue(ComplexStruct(local_i++)));
        } else {
            // Consumer
            ComplexStruct item;
            benchmark::DoNotOptimize(q.dequeue(item));
        }
    }
}
BENCHMARK(BM_EnqueueDequeue)->Threads(2)->UseRealTime();

static void BM_LockFreeQueue_ProdCons(benchmark::State& state) {
    static LockFreeQueue<ComplexStruct, 262144> q;      // shared across threads
    static std::atomic<bool> start{false};

    int value = 0;
    size_t ops = 0;

    // synchronize start
    if (state.thread_index() == 0) {
        start.store(true, std::memory_order_release);
    } else {
        while (!start.load(std::memory_order_acquire)) {}
    }

    for (auto _ : state) {
        if (state.thread_index() == 0) {
            // Producer
            if (q.enqueue(value)) {
                ++value;
                ++ops;
            }
        } else {
            // Consumer
            ComplexStruct cs;
            if (q.dequeue(cs)) {
                ++ops;
            }
        }
    }

    state.SetItemsProcessed(ops);
}
BENCHMARK(BM_LockFreeQueue_ProdCons)->Threads(2)->UseRealTime();

BENCHMARK_MAIN();