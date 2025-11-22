#include "MemoryPool.hpp"
#include <iostream>
#include <gtest/gtest.h>
/*
Unit tests for MemoryPool class.

*/
namespace {
    class TestObject {
    public:
        TestObject() {}; // TODO : Current implementation requires default constructor.

        explicit TestObject(int a, std::string b) : _a(a), _b(b) {}
        int a() const { return _a; }
        std::string b() const { return _b; }
    private:  
        int _a;
        std::string _b;
    };
}

TEST(MemoryPoolTest, AllocateAndCheckObjects) {
    MemoryPool<TestObject> pool(2);
    TestObject* obj1 = pool.allocate(1, "one");
    TestObject* obj2 = pool.allocate(2, "two");

    EXPECT_EQ(obj1->a(), 1);
    EXPECT_EQ(obj1->b(), "one");
    EXPECT_EQ(obj2->a(), 2);
    EXPECT_EQ(obj2->b(), "two");
}

TEST(MemoryPoolTest, ReuseDeallocatedObjects) {
    MemoryPool<TestObject> pool(2);
    TestObject* obj1 = pool.allocate(1, "one");
    pool.deallocate(obj1);
    TestObject* obj2 = pool.allocate(2, "two");

    EXPECT_EQ(obj1, obj2); // Should reuse the same memory location
    EXPECT_EQ(obj2->a(), 2);
    EXPECT_EQ(obj2->b(), "two");
}

TEST(MemoryPoolTest, PoolExpansion) {
    MemoryPool<TestObject> pool(2);
    pool.allocate(1, "one");
    EXPECT_EQ(pool.CurrentSize(), 1);
    EXPECT_EQ(pool.PoolSize(), 2); 
    pool.allocate(2, "two");
    EXPECT_EQ(pool.CurrentSize(), 2);
    EXPECT_EQ(pool.PoolSize(), 2); 
    pool.allocate(3, "three"); // This should trigger expansion
    EXPECT_EQ(pool.CurrentSize(), 3);
    EXPECT_EQ(pool.PoolSize(), 4); // Pool size should have doubled

}