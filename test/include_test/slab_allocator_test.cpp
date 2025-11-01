#include <gtest/gtest.h>

#include <iostream>

#include "util/slaballocator.hpp"

TEST(SlabAllocatorTest, Basic) {
    static int createCount;
    static int destroyCount;

    struct TestObject {
        TestObject() { createCount++; }
        explicit TestObject(int v) : TestObject() { data = v; }
        ~TestObject() { destroyCount++; }

        int data{0};
    };

    static constexpr int SLAB_SIZE = 32;
    static constexpr int SLAB_COUNT = 32;
    auto &pool = SlabAllocator<TestObject, SLAB_SIZE>::GetInst();

    std::vector<TestObject *> ptrVec;
    for (int i = 0; i < SLAB_COUNT; i++) {
        for (int j = 0; j < SLAB_SIZE; j++) {
            TestObject *obj = pool.allocate(j + i * SLAB_SIZE);
            ASSERT_EQ(pool.partial_list_size(), j == SLAB_SIZE - 1 ? 0 : 1);
            ASSERT_EQ(pool.free_list_size(), 0);
            ASSERT_EQ(pool.full_list_size(), i + (j == SLAB_SIZE - 1 ? 1 : 0));
            ASSERT_EQ(
                pool.free_list_size() + pool.full_list_size() + pool.partial_list_size(),
                i + 1);
            ptrVec.push_back(obj);
        }
    }

    for (int i = 0; i < ptrVec.size(); i++) {
        int group = i / SLAB_SIZE;
        ASSERT_EQ(ptrVec[i]->data, i);
        pool.deallocate(ptrVec[i]);

        ASSERT_EQ(pool.partial_list_size(), (i + 1) % SLAB_SIZE != 0);
        ASSERT_EQ(pool.free_list_size(), group + ((i + 1) % SLAB_SIZE == 0));
        ASSERT_EQ(pool.full_list_size(), SLAB_COUNT - group - 1);
        ASSERT_EQ(
            pool.free_list_size() + pool.full_list_size() + pool.partial_list_size(),
            SLAB_COUNT);
    }

    ASSERT_EQ(createCount, SLAB_SIZE * SLAB_COUNT);
    ASSERT_EQ(destroyCount, SLAB_SIZE * SLAB_COUNT);
};