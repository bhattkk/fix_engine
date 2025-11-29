#include "StaticMemoryPool.hpp"
#include <iostream>
 #include <gtest/gtest.h>

//  Unit tests for MemoryPool classes.

namespace {
    class TestObject {
    public:
        explicit TestObject(int integer, std::string str) : _intParam(integer), _strParam(str) {}
        int intParam() const { return _intParam; }
        std::string strParam() const { return _strParam; }
        ~TestObject()
        {
            destructorCallCount++;
        }

        static int destructorCallCount;
    private:  
        int _intParam;
        std::string _strParam;
    };

    int TestObject::destructorCallCount = 0;
}

TEST(MemoryPoolTest, StaticMemoryPoolAllocationDeallocation) {

    // Reset destructor call count
    TestObject::destructorCallCount = 0;
    StaticMemoryPool<TestObject, 2> pool;

    // 1. Allocate two objects and verify their parameters
    TestObject* obj1 = pool.allocate(1, "one");
    TestObject* obj2 = pool.allocate(2, "two");

    EXPECT_EQ(obj1->intParam(), 1);
    EXPECT_EQ(obj1->strParam(), "one");

    EXPECT_EQ(obj2->intParam(), 2);
    EXPECT_EQ(obj2->strParam(), "two");

    // 2. Deallocate one object and verify destructor call count
    pool.deallocate(obj1);
    EXPECT_EQ(TestObject::destructorCallCount, 1);
    pool.deallocate(obj2);
    EXPECT_EQ(TestObject::destructorCallCount, 2);

    // 3. Allocate two more objects to check reuse of deallocated slots
    TestObject* obj3 = pool.allocate(3, "three");
    EXPECT_EQ(obj3->intParam(), 3);
    EXPECT_EQ(obj3->strParam(), "three");

    TestObject* obj4 = pool.allocate(4, "four");
    EXPECT_EQ(obj4->intParam(), 4);
    EXPECT_EQ(obj4->strParam(), "four");

    // 4. Attempt to allocate a third object and expect an exception
    try
    {
        pool.allocate(5, "five");
    }
    catch(const std::exception& e)
    {
        EXPECT_STREQ(e.what(), "StaticMemoryPool: No free memory available for allocation");
    }
    
    // 5. Deallocate one object and now allocation should succeed
    pool.deallocate(obj3);
    EXPECT_EQ(TestObject::destructorCallCount, 3);
    
    TestObject* obj5 = pool.allocate(5, "five");
    EXPECT_EQ(obj5->intParam(), 5);
    EXPECT_EQ(obj5->strParam(), "five");

    // 6. Clean up remaining objects
    pool.deallocate(obj4);
    EXPECT_EQ(TestObject::destructorCallCount, 4);  

    pool.deallocate(obj5);
    EXPECT_EQ(TestObject::destructorCallCount, 5);
    
}