#include "util/objectpool.hpp"

#include <gtest/gtest.h>

TEST(ObjectPoolTest, ObjectPoolTest) {
    static int createCount;
    static int destroyCount;

    struct TestObject {
        TestObject() { createCount++; }
        ~TestObject() { destroyCount++; }

        int getData() { return data; }

        int data{0};
    };

    {
        auto &pool = ObjectPool<TestObject>::GetInst();
        EXPECT_EQ(createCount, 32);
        EXPECT_EQ(pool.size(), 32);

        auto p1 = pool.acquire();
        EXPECT_EQ(pool.size(), 31);
        EXPECT_EQ(p1->getData(), 0);
        p1->data = 10;
        pool.release(std::move(p1));
        EXPECT_EQ(pool.size(), 32);

        // reuse the object
        {
            auto p2 = pool.acquire();
            EXPECT_EQ(pool.size(), 31);
            EXPECT_EQ(p2->getData(), 10);
            p2->data = 20;
            EXPECT_EQ(p2->getData(), 20);
            // destroy p2
        }

        EXPECT_EQ(pool.size(), 31);
    }
    EXPECT_EQ(createCount, 32);
    EXPECT_EQ(destroyCount, 1);
};