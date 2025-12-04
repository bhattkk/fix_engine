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
    StaticMemoryPool<TestObject, 128> pool;

    //1. Allocate two objects and verify their parameters
    TestObject* obj1 = pool.alloc(1, "number_1");
    TestObject* obj2 = pool.alloc(2, "number_2");

    EXPECT_EQ(obj1->intParam(), 1);
    EXPECT_EQ(obj1->strParam(), "number_1");

    EXPECT_EQ(obj2->intParam(), 2);
    EXPECT_EQ(obj2->strParam(), "number_2");

    // 2. Deallocate one object and verify destructor call count
    pool.free(obj1);
    EXPECT_EQ(TestObject::destructorCallCount, 1);
    pool.free(obj2);
    EXPECT_EQ(TestObject::destructorCallCount, 2);

    // 3. allocate 128 objects to fill the pool
    std::vector<TestObject*> allocatedObjects;
    for (size_t i = 0; i < 128; ++i) {
        allocatedObjects.push_back(pool.alloc(i, "number_" + std::to_string(i)));
    }
    EXPECT_EQ(allocatedObjects.size(), 128);
    for (size_t i = 0; i < 128; ++i) {
        EXPECT_EQ(allocatedObjects[i]->intParam(), i);
        EXPECT_EQ(allocatedObjects[i]->strParam(), "number_" + std::to_string(i));
    }

    // 4. Attempt to allocate a new object and expect an exception
    try
    {
        pool.alloc(129, "number_129");
    }
    catch(const std::exception& e)
    {
        EXPECT_STREQ(e.what(), "StaticMemoryPool: No free memory available for allocation");
    }
    
    // 5. Deallocate one object and now allocation should succeed
    pool.free(allocatedObjects[0]);
    EXPECT_EQ(TestObject::destructorCallCount, 3);
    
    TestObject* obj129 = pool.alloc(129, "number_129");
    EXPECT_EQ(obj129->intParam(), 129);
    EXPECT_EQ(obj129->strParam(), "number_129");

    // 6. Try allocating another object and expect an exception
    try
    {
        pool.alloc(130, "number_130");
    }
    catch(const std::exception& e)
    {
        EXPECT_STREQ(e.what(), "StaticMemoryPool: No free memory available for allocation");
    }

    // 7. Try deallocating a null pointer and expect an exception
    try
    {
        pool.free(nullptr);
    }
    catch(const std::exception& e)
    {
        EXPECT_STREQ(e.what(), "StaticMemoryPool: Cannot deallocate a null pointer");
    }

    EXPECT_EQ(TestObject::destructorCallCount, 3);
    // 8. Try deallocating an out-of-range pointer and expect an exception
    TestObject invalidObj(0, "invalid");
    try
    {
        pool.free(&invalidObj);
    }
    catch(const std::exception& e)
    {
        EXPECT_STREQ(e.what(), "StaticMemoryPool: Pointer out of range for deallocation");
    }   
    EXPECT_EQ(TestObject::destructorCallCount, 3); // invalidObj destructor not called yet
    int currentDestructorCount = TestObject::destructorCallCount;
    // 9. Clean up remaining objects
    for (size_t i = 1; i < 128; ++i) {
            pool.free(allocatedObjects[i]);
            currentDestructorCount++;
            EXPECT_EQ(TestObject::destructorCallCount, currentDestructorCount);
    }
    pool.free(obj129);
    currentDestructorCount++;
    EXPECT_EQ(TestObject::destructorCallCount, currentDestructorCount);    
}

TEST(MemoryPoolTest, HeapMemoryPoolBasicAllocationDeallocation) {
    // This test is a placeholder for HeapMemoryPool tests.
    // Implement similar tests for HeapMemoryPool as done for StaticMemoryPool.
    SUCCEED();
}