#include <gtest/gtest.h>
#include "LockFreeQueue.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

namespace {
    class ComplexStruct {
        int a;
        std::string c1;
        std::string c2;
    public:
        ComplexStruct() = default;
        ComplexStruct(int val) 
        :a(val),
        c1("Test" + std::to_string(val)),
        c2("Data" + std::to_string(val))
        {}

        int getId() const { return a; }

        ~ComplexStruct() = default;
        // Making sure no implicit copy/move operations.
        ComplexStruct(const ComplexStruct&) = delete;
        ComplexStruct& operator=(const ComplexStruct&) = delete;
        ComplexStruct(ComplexStruct&&) = delete;
        ComplexStruct& operator=(ComplexStruct&&) = delete;
    };
} 

TEST(LockFreeQueueTest, BasicEnqueueDequeue) {
    LockFreeQueue<ComplexStruct, 4> queue;

    //1. Enqueue elements into the queue
    // Enqueue up to capacity - 1. Due to the design, one slot is left empty to distinguish full vs empty.
    {
        for (int i = 1; i <= 3; ++i) {
            auto result = queue.enqueue(ComplexStruct(i));
            EXPECT_TRUE(result.has_value());
        }
    }

    //2. Attempt to enqueue into a full queue
    {
        auto result = queue.enqueue(ComplexStruct(4));
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), "LockFreeQueue: Queue is full");
    }

    //3. Dequeue elements and verify order
    {
        for (int i = 1; i <= 3; ++i) {
            ComplexStruct value(0);
            auto result = queue.dequeue(value);
            EXPECT_TRUE(result.has_value());
            EXPECT_EQ(value.getId(), i);
        }
    }

    //4. Queue should be empty now
    {
        ComplexStruct value(0);
        auto result = queue.dequeue(value);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), "LockFreeQueue: Queue is empty");
    }
}


TEST(LockFreeQueueTest, StressRaceTest) {
    LockFreeQueue<ComplexStruct, 1024> q;

    std::atomic<bool> start{false};
    std::atomic<bool> stop{false};

    std::thread producer([&] {
        while (!start.load(std::memory_order_relaxed));
        int x = 0;
        while (!stop.load(std::memory_order_relaxed)) {
            q.enqueue(ComplexStruct(x++));
        }
    });

    std::thread consumer([&] {
        while (!start.load(std::memory_order_relaxed));
        while (!stop.load(std::memory_order_relaxed)) {
            ComplexStruct x(0);
            q.dequeue(x);
        }
    });

    start.store(true, std::memory_order_relaxed);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    stop.store(true, std::memory_order_relaxed);

    producer.join();
    consumer.join();
}