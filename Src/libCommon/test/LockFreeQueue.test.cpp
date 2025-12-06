#include <gtest/gtest.h>
#include "LockFreeQueue.hpp"
#include <iostream>

TEST(LockFreeQueueTest, BasicEnqueueDequeue) {
    LockFreeQueue<int, 4> queue;

    //1. Enqueue elements into the queue
    {
        for (int i = 1; i <= 4; ++i) {
            std::cout << "Enqueuing: " << i << std::endl;
            auto result = queue.enqueue(i);
            EXPECT_TRUE(result.has_value());
        }
    }

    //2. Attempt to enqueue into a full queue
    {
        auto result = queue.enqueue(5);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), "LockFreeQueue: Queue is full");
    }

    //3. Dequeue elements and verify order
    {
        for (int i = 1; i <= 4; ++i) {
            int value;
            auto result = queue.dequeue(value);
            EXPECT_TRUE(result.has_value());
            EXPECT_EQ(value, i);
        }
    }

    //4. Queue should be empty now
    {
        int value;
        auto result = queue.dequeue(value);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), "LockFreeQueue: Queue is empty");
    }
}