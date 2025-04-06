#include "book/orderbook.hpp"

#include <gtest/gtest.h>

TEST(MapBasedOrderBookTest, LinkedListLevel) {
    MapBasedPriceLevelOrderBook<std::list<Order>> orderBook;
    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(MapBasedOrderBookTest, SetLevel) {
    MapBasedPriceLevelOrderBook<std::set<Order>> orderBook;
    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(MapBasedOrderBookTest, UnorderedSetLevel) {
    MapBasedPriceLevelOrderBook<std::unordered_set<Order, Order::Hash, Order::Equal>>
        orderBook;

    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(MapBasedOrderBookTest, MultisetLevel) {
    MapBasedPriceLevelOrderBook<std::multiset<Order>> orderBook;

    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(MapBasedOrderBookTest, UnorderedMultisetLevel) {
    MapBasedPriceLevelOrderBook<std::unordered_multiset<Order, Order::Hash, Order::Equal>>
        orderBook;

    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(VectorBasedOrderBookTest, LinkedListLevel) {
    VectorBasedOrderBook<std::list<Order>> orderBook;
    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(VectorBasedOrderBookTest, SetLevel) {
    MapBasedPriceLevelOrderBook<std::set<Order>> orderBook;
    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(VectorBasedOrderBookTest, UnorderedSetLevel) {
    MapBasedPriceLevelOrderBook<std::unordered_set<Order, Order::Hash, Order::Equal>>
        orderBook;

    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(VectorBasedOrderBookTest, MultisetLevel) {
    MapBasedPriceLevelOrderBook<std::multiset<Order>> orderBook;

    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(VectorBasedOrderBookTest, UnorderedMultisetLevel) {
    MapBasedPriceLevelOrderBook<std::unordered_multiset<Order, Order::Hash, Order::Equal>>
        orderBook;

    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // test bid
    orderBook.addOrder(Order{"1", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "1");

    orderBook.addOrder(Order{"3", "a", "b", "AAPL", Order::Side::Buy, 100, 10});
    EXPECT_FALSE(orderBook.isBidEmpty());

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.addOrder(Order{"4", "a", "b", "AAPL", Order::Side::Buy, 100, 11});
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.orderID, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(Order{"2", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "2");

    orderBook.addOrder(Order{"5", "a", "b", "AAPL", Order::Side::Sell, 100, 10});
    EXPECT_FALSE(orderBook.isAskEmpty());

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.addOrder(Order{"6", "a", "b", "AAPL", Order::Side::Sell, 100, 9});
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.orderID, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}