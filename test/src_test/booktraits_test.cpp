#include "book/booktraits.hpp"

#include <gtest/gtest.h>

TEST(BookTraitsTest, LevelContainerTraits) {
    {
        std::vector<Order> v;

        auto it = LevelContainerTraits<decltype(v)>::insert(
            v, Order("1", "a", "b", "AAPL", Order::Side::Buy, 100, 100.0));
        EXPECT_EQ(it, std::prev(v.end()));
        auto it2 = LevelContainerTraits<decltype(v)>::insert(
            v, Order("2", "a", "b", "AAPL", Order::Side::Buy, 100, 100.0));
        EXPECT_EQ(it2, std::prev(v.end()));
    }

    {
        std::list<Order> v;

        auto it = LevelContainerTraits<decltype(v)>::insert(
            v, Order("1", "a", "b", "AAPL", Order::Side::Buy, 100, 100.0));
        EXPECT_EQ(it, std::prev(v.end()));
        auto it2 = LevelContainerTraits<decltype(v)>::insert(
            v, Order("2", "a", "b", "AAPL", Order::Side::Buy, 100, 100.0));
        EXPECT_EQ(it2, std::prev(v.end()));
    }

    {
        std::deque<Order> v;

        auto it = LevelContainerTraits<decltype(v)>::insert(
            v, Order("1", "a", "b", "AAPL", Order::Side::Buy, 100, 100.0));
        EXPECT_EQ(it, std::prev(v.end()));
        auto it2 = LevelContainerTraits<decltype(v)>::insert(
            v, Order("2", "a", "b", "AAPL", Order::Side::Buy, 100, 100.0));
        EXPECT_EQ(it2, std::prev(v.end()));
    }

    {
        std::set<Order> s;
        auto it = LevelContainerTraits<decltype(s)>::insert(
            s, Order("1", "a", "b", "AAPL", Order::Side::Buy, 100, 100.0));
        EXPECT_EQ(it, s.begin());
        auto it2 = LevelContainerTraits<decltype(s)>::insert(
            s, Order("2", "a", "b", "AAPL", Order::Side::Buy, 100, 99.0));
        EXPECT_EQ(it2, s.begin());
    }

    {
        std::multiset<Order> s;
        auto it = LevelContainerTraits<decltype(s)>::insert(
            s, Order("1", "a", "b", "AAPL", Order::Side::Buy, 100, 100.0));
        EXPECT_EQ(it, s.begin());
        auto it2 = LevelContainerTraits<decltype(s)>::insert(
            s, Order("2", "a", "b", "AAPL", Order::Side::Buy, 100, 99.0));
        EXPECT_EQ(it2, s.begin());
    }

    {
        std::unordered_set<Order, Order::Hash, Order::Equal> s;
        auto it = LevelContainerTraits<decltype(s)>::insert(
            s, Order("1", "a", "b", "AAPL", Order::Side::Buy, 100, 100.0));
        EXPECT_EQ(it, s.begin());

        auto it2 = LevelContainerTraits<decltype(s)>::insert(
            s, Order("2", "a", "b", "AAPL", Order::Side::Buy, 100, 99.0));
        // std::prev(s.end()); // wrong. unordered_set iterator is not bidirectional
    }
}