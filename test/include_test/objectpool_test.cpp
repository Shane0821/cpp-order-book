#include "util/objectpool.hpp"

#include <gtest/gtest.h>

#include <iostream>

static int createCount;
static int destroyCount;

struct TestObject {
    TestObject() { createCount++; }
    explicit TestObject(int v) : TestObject() { data = v; }
    ~TestObject() { destroyCount++; }

    int getData() { return data; }

    int data{0};
};

TEST(ObjectPoolTest, Basic) {
    createCount = 0;
    destroyCount = 0;

    static constexpr size_t INITIAL_SIZE = 32;
    auto &pool = ObjectPool<TestObject, INITIAL_SIZE>::GetInst();
    EXPECT_EQ(pool.size(), INITIAL_SIZE);

    auto p1 = pool.allocate();
    EXPECT_EQ(pool.size(), INITIAL_SIZE - 1);
    EXPECT_EQ(p1->getData(), 0);
    p1->data = 10;
    pool.deallocate(p1);
    EXPECT_EQ(pool.size(), INITIAL_SIZE);

    auto p2 = pool.allocate(10);
    EXPECT_EQ(pool.size(), INITIAL_SIZE - 1);
    EXPECT_EQ(p2->getData(), 10);
    p2->data = 20;
    EXPECT_EQ(p2->getData(), 20);
    pool.deallocate(p2);
    EXPECT_EQ(pool.size(), INITIAL_SIZE);

    EXPECT_EQ(createCount, 2);
    EXPECT_EQ(destroyCount, 2);
};

TEST(ObjectPoolTest, Mixed) {
    createCount = 0;
    destroyCount = 0;

    static constexpr size_t INITIAL_SIZE = 32;
    auto &pool = ObjectPool<TestObject, INITIAL_SIZE>::GetInst();
    EXPECT_EQ(pool.size(), INITIAL_SIZE);

    std::vector<TestObject *> objs;
    for (int i = 0; i < INITIAL_SIZE; i++) {
        auto p1 = pool.allocate(i);
        EXPECT_EQ(pool.size(), INITIAL_SIZE - i - 1);
        EXPECT_EQ(p1->getData(), i);
        objs.push_back(p1);
    }

    for (int i = INITIAL_SIZE; i < INITIAL_SIZE * 2; i++) {
        auto p1 = pool.allocate(i);
        EXPECT_EQ(pool.size(), 0);
        EXPECT_EQ(p1->getData(), i);
        objs.push_back(p1);
    }

    for (int i = 0; i < INITIAL_SIZE; i++) {
        pool.deallocate(objs.back());
        objs.pop_back();
        EXPECT_EQ(pool.size(), 0);
    }

    for (int i = 0; i < INITIAL_SIZE; i++) {
        pool.deallocate(objs.back());
        objs.pop_back();
        EXPECT_EQ(pool.size(), i + 1);
    }

    EXPECT_EQ(createCount, INITIAL_SIZE * 2);
    EXPECT_EQ(destroyCount, INITIAL_SIZE * 2);
};