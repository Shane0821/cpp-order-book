#include "book/level_traits.hpp"

#include <gtest/gtest.h>

#include "book/order.h"
#include "book/order_cmp.h"

TEST(LevelTraitsTest, TraitsInsert) {
    static_assert(has_valid_insert_v<std::vector<Order>>, "vector has valid insert");
    static_assert(has_valid_insert_v<std::deque<Order *>>, "deque has valid insert");
    static_assert(has_valid_insert_v<std::list<Order *>>, "list has valid insert");
    static_assert(has_valid_insert_v<std::set<Order *>>, "set has valid insert");
    // static_assert(has_valid_insert_v<std::unordered_set<Order, Order::Hash,
    // Order::Equal>>,
    //               "unordered_set has valid insert");
}

TEST(LevelTraitsTest, ListInsert) {
    std::list<Order> v;

    auto it = LevelContainerTraits<decltype(v)>::insert(
        v, Order{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 100});
    EXPECT_EQ(it, std::prev(v.end()));
    auto it2 = LevelContainerTraits<decltype(v)>::insert(
        v, Order{"2", OrderType::GoodTillCancel, Side::Buy, 99.0, 100});
    EXPECT_EQ(it2, std::prev(v.end()));
}

TEST(LevelTraitsTest, PtrListInsert) {
    std::list<Order *> v;
    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 100};
    auto it = LevelContainerTraits<decltype(v)>::insert(v, &order1);
    EXPECT_EQ(it, std::prev(v.end()));
    Order order2{"2", OrderType::GoodTillCancel, Side::Buy, 99.0, 100};
    auto it2 = LevelContainerTraits<decltype(v)>::insert(v, &order2);
    EXPECT_EQ(it2, std::prev(v.end()));
}

TEST(LevelTraitsTest, DequeInsert) {
    std::deque<Order> v;

    auto it = LevelContainerTraits<decltype(v)>::insert(
        v, Order{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 100});
    EXPECT_EQ(it, std::prev(v.end()));
    auto it2 = LevelContainerTraits<decltype(v)>::insert(
        v, Order{"2", OrderType::GoodTillCancel, Side::Buy, 99.0, 100});
    EXPECT_EQ(it2, std::prev(v.end()));
}

TEST(LevelTraitsTest, PtrDequeInsert) {
    std::deque<Order *> v;
    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 100};
    auto it = LevelContainerTraits<decltype(v)>::insert(v, &order1);
    EXPECT_EQ(it, std::prev(v.end()));
    Order order2{"2", OrderType::GoodTillCancel, Side::Buy, 99.0, 100};
    auto it2 = LevelContainerTraits<decltype(v)>::insert(v, &order2);
    EXPECT_EQ(it2, std::prev(v.end()));
}

TEST(LevelTraitsTest, SetInsert) {
    std::set<Order, OrderComparison> s;

    auto it = LevelContainerTraits<decltype(s)>::insert(
        s, Order{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 100});
    EXPECT_EQ(it, s.begin());
    auto it2 = LevelContainerTraits<decltype(s)>::insert(
        s, Order{"2", OrderType::GoodTillCancel, Side::Buy, 99.0, 100});
    EXPECT_EQ(it2, ++s.begin());
}

TEST(LevelTraitsTest, PtrSetInsert) {
    std::set<Order *, OrderComparison> s;

    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 100};
    auto it = LevelContainerTraits<decltype(s)>::insert(s, &order1);
    EXPECT_EQ(it, s.begin());
    Order order2{"2", OrderType::GoodTillCancel, Side::Buy, 99.0, 100};
    auto it2 = LevelContainerTraits<decltype(s)>::insert(s, &order2);
    EXPECT_EQ(it2, ++s.begin());
}

TEST(LevelTraitsTest, MultisetInsert) {
    std::multiset<Order, OrderComparison> s;

    auto it = LevelContainerTraits<decltype(s)>::insert(
        s, Order{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 100});
    EXPECT_EQ(it, s.begin());
    auto it2 = LevelContainerTraits<decltype(s)>::insert(
        s, Order{"2", OrderType::GoodTillCancel, Side::Buy, 99.0, 100});
    EXPECT_EQ(it2, ++s.begin());
}

TEST(LevelTraitsTest, PtrMultisetInsert) {
    std::multiset<Order *, OrderComparison> s;

    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 100};
    auto it = LevelContainerTraits<decltype(s)>::insert(s, &order1);
    EXPECT_EQ(it, s.begin());
    Order order2{"2", OrderType::GoodTillCancel, Side::Buy, 99.0, 100};
    auto it2 = LevelContainerTraits<decltype(s)>::insert(s, &order2);
    EXPECT_EQ(it2, ++s.begin());
}

// TEST(LevelTraitsTest, UnorderedSetInsert) {
//     std::unordered_set<Order, Order::Hash, Order::Equal> s;

//     auto it = LevelContainerTraits<decltype(s)>::insert(
//         s, Order{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 100});
//     EXPECT_EQ(it, s.begin());

//     auto it2 = LevelContainerTraits<decltype(s)>::insert(
//         s, Order{"2", OrderType::GoodTillCancel, Side::Buy, 99.0, 100});
//     // std::prev(s.end()); // wrong. unordered_set iterator is not bidirectional
// }