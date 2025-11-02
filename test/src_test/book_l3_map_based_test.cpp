#include <gtest/gtest.h>

#include "book/book_l3_map.hpp"

TEST(MapBasedL3OrderBookTest, LinkedListLevel) {
    MapBasedL3OrderBook<std::list<Order *>> orderBook;
    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // bid orders
    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order3{"3", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order4{"4", OrderType::GoodTillCancel, Side::Buy, 11.0, 100};

    // ask orders
    Order order2{"2", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order5{"5", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order6{"6", OrderType::GoodTillCancel, Side::Sell, 9.0, 100};

    // test bid
    orderBook.addOrder(&order1);
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "1");

    orderBook.addOrder(&order3);
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "1");

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "3");

    orderBook.addOrder(&order4);
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(&order2);
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "2");

    orderBook.addOrder(&order5);
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "2");

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "5");

    orderBook.addOrder(&order6);
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(MapBasedL3OrderBookTest, LinkedListLevelMatch) {
    MapBasedL3OrderBook<std::list<Order *>> orderBook;

    // bid orders
    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order3{"3", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order4{"4", OrderType::GoodTillCancel, Side::Buy, 11.0, 100};

    // ask orders
    Order order2{"2", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order5{"5", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order6{"6", OrderType::GoodTillCancel, Side::Sell, 9.0, 50};

    Trades trades;
    trades = orderBook.addOrder(&order1);
    ASSERT_TRUE(trades.empty());
    trades = orderBook.addOrder(&order3);
    ASSERT_TRUE(trades.empty());
    trades = orderBook.addOrder(&order4);
    ASSERT_TRUE(trades.empty());

    trades = orderBook.addOrder(&order2);
    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(trades[0].bidTrade_.orderId_, "4");
    ASSERT_EQ(trades[0].bidTrade_.price_, 11.0);
    ASSERT_EQ(trades[0].bidTrade_.quantity_, 100);
    ASSERT_EQ(trades[0].askTrade_.orderId_, "2");
    ASSERT_EQ(trades[0].askTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].askTrade_.quantity_, 100);

    trades = orderBook.addOrder(&order5);
    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(trades[0].bidTrade_.orderId_, "1");
    ASSERT_EQ(trades[0].bidTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].bidTrade_.quantity_, 100);
    ASSERT_EQ(trades[0].askTrade_.orderId_, "5");
    ASSERT_EQ(trades[0].askTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].askTrade_.quantity_, 100);

    trades = orderBook.addOrder(&order6);
    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(trades[0].bidTrade_.orderId_, "3");
    ASSERT_EQ(trades[0].bidTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].bidTrade_.quantity_, 50);
    ASSERT_EQ(trades[0].askTrade_.orderId_, "6");
    ASSERT_EQ(trades[0].askTrade_.price_, 9.0);
    ASSERT_EQ(trades[0].askTrade_.quantity_, 50);

    ASSERT_TRUE(orderBook.isAskEmpty());

    ASSERT_FALSE(orderBook.isBidEmpty());
    auto bid = orderBook.getBestBid();
    ASSERT_EQ(bid->getPrice(), 10.0);
    ASSERT_EQ(bid->getFilledQuantity(), 50);
    ASSERT_EQ(bid->getInitialQuantity(), 100);
    ASSERT_EQ(bid->getOrderId(), "3");
}

TEST(MapBasedL3OrderBookTest, SetLevel) {
    MapBasedL3OrderBook<std::set<Order *, OrderCompare>> orderBook;
    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // bid orders
    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order3{"3", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order4{"4", OrderType::GoodTillCancel, Side::Buy, 11.0, 100};

    // ask orders
    Order order2{"2", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order5{"5", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order6{"6", OrderType::GoodTillCancel, Side::Sell, 9.0, 100};

    // test bid
    orderBook.addOrder(&order1);
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "1");

    orderBook.addOrder(&order3);
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "1");

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "3");

    orderBook.addOrder(&order4);
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(&order2);
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "2");

    orderBook.addOrder(&order5);
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "2");

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "5");

    orderBook.addOrder(&order6);
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(MapBasedL3OrderBookTest, SetLevelMatch) {
    MapBasedL3OrderBook<std::set<Order *, OrderCompare>> orderBook;

    // bid orders
    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order3{"3", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order4{"4", OrderType::GoodTillCancel, Side::Buy, 11.0, 100};

    // ask orders
    Order order2{"2", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order5{"5", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order6{"6", OrderType::GoodTillCancel, Side::Sell, 9.0, 50};

    Trades trades;
    trades = orderBook.addOrder(&order1);
    ASSERT_TRUE(trades.empty());
    trades = orderBook.addOrder(&order3);
    ASSERT_TRUE(trades.empty());
    trades = orderBook.addOrder(&order4);
    ASSERT_TRUE(trades.empty());

    trades = orderBook.addOrder(&order2);
    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(trades[0].bidTrade_.orderId_, "4");
    ASSERT_EQ(trades[0].bidTrade_.price_, 11.0);
    ASSERT_EQ(trades[0].bidTrade_.quantity_, 100);
    ASSERT_EQ(trades[0].askTrade_.orderId_, "2");
    ASSERT_EQ(trades[0].askTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].askTrade_.quantity_, 100);

    trades = orderBook.addOrder(&order5);
    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(trades[0].bidTrade_.orderId_, "1");
    ASSERT_EQ(trades[0].bidTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].bidTrade_.quantity_, 100);
    ASSERT_EQ(trades[0].askTrade_.orderId_, "5");
    ASSERT_EQ(trades[0].askTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].askTrade_.quantity_, 100);

    trades = orderBook.addOrder(&order6);
    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(trades[0].bidTrade_.orderId_, "3");
    ASSERT_EQ(trades[0].bidTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].bidTrade_.quantity_, 50);
    ASSERT_EQ(trades[0].askTrade_.orderId_, "6");
    ASSERT_EQ(trades[0].askTrade_.price_, 9.0);
    ASSERT_EQ(trades[0].askTrade_.quantity_, 50);

    ASSERT_TRUE(orderBook.isAskEmpty());

    ASSERT_FALSE(orderBook.isBidEmpty());
    auto bid = orderBook.getBestBid();
    ASSERT_EQ(bid->getPrice(), 10.0);
    ASSERT_EQ(bid->getFilledQuantity(), 50);
    ASSERT_EQ(bid->getInitialQuantity(), 100);
    ASSERT_EQ(bid->getOrderId(), "3");
}

TEST(MapBasedL3OrderBookTest, MultisetLevel) {
    MapBasedL3OrderBook<std::multiset<Order *, OrderCompare>> orderBook;

    EXPECT_TRUE(orderBook.isBidEmpty());
    EXPECT_TRUE(orderBook.isAskEmpty());

    // bid orders
    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order3{"3", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order4{"4", OrderType::GoodTillCancel, Side::Buy, 11.0, 100};

    // ask orders
    Order order2{"2", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order5{"5", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order6{"6", OrderType::GoodTillCancel, Side::Sell, 9.0, 100};

    // test bid
    orderBook.addOrder(&order1);
    EXPECT_FALSE(orderBook.isBidEmpty());
    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "1");

    orderBook.addOrder(&order3);
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "1");

    orderBook.cancelOrder("1");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "3");

    orderBook.addOrder(&order4);
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "4");

    orderBook.cancelOrder("4");
    EXPECT_FALSE(orderBook.isBidEmpty());
    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid->orderId_, "3");

    orderBook.cancelOrder("3");
    EXPECT_TRUE(orderBook.isBidEmpty());

    // test ask
    orderBook.addOrder(&order2);
    EXPECT_FALSE(orderBook.isAskEmpty());
    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "2");

    orderBook.addOrder(&order5);
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "2");

    orderBook.cancelOrder("2");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "5");

    orderBook.addOrder(&order6);
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "6");

    orderBook.cancelOrder("6");
    EXPECT_FALSE(orderBook.isAskEmpty());
    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk->orderId_, "5");

    orderBook.cancelOrder("5");
    EXPECT_TRUE(orderBook.isAskEmpty());
}

TEST(MapBasedL3OrderBookTest, MultiSetLevelMatch) {
    MapBasedL3OrderBook<std::multiset<Order *, OrderCompare>> orderBook;

    // bid orders
    Order order1{"1", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order3{"3", OrderType::GoodTillCancel, Side::Buy, 10.0, 100};
    Order order4{"4", OrderType::GoodTillCancel, Side::Buy, 11.0, 100};

    // ask orders
    Order order2{"2", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order5{"5", OrderType::GoodTillCancel, Side::Sell, 10.0, 100};
    Order order6{"6", OrderType::GoodTillCancel, Side::Sell, 9.0, 50};

    Trades trades;
    trades = orderBook.addOrder(&order1);
    ASSERT_TRUE(trades.empty());
    trades = orderBook.addOrder(&order3);
    ASSERT_TRUE(trades.empty());
    trades = orderBook.addOrder(&order4);
    ASSERT_TRUE(trades.empty());

    trades = orderBook.addOrder(&order2);
    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(trades[0].bidTrade_.orderId_, "4");
    ASSERT_EQ(trades[0].bidTrade_.price_, 11.0);
    ASSERT_EQ(trades[0].bidTrade_.quantity_, 100);
    ASSERT_EQ(trades[0].askTrade_.orderId_, "2");
    ASSERT_EQ(trades[0].askTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].askTrade_.quantity_, 100);

    trades = orderBook.addOrder(&order5);
    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(trades[0].bidTrade_.orderId_, "1");
    ASSERT_EQ(trades[0].bidTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].bidTrade_.quantity_, 100);
    ASSERT_EQ(trades[0].askTrade_.orderId_, "5");
    ASSERT_EQ(trades[0].askTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].askTrade_.quantity_, 100);

    trades = orderBook.addOrder(&order6);
    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(trades[0].bidTrade_.orderId_, "3");
    ASSERT_EQ(trades[0].bidTrade_.price_, 10.0);
    ASSERT_EQ(trades[0].bidTrade_.quantity_, 50);
    ASSERT_EQ(trades[0].askTrade_.orderId_, "6");
    ASSERT_EQ(trades[0].askTrade_.price_, 9.0);
    ASSERT_EQ(trades[0].askTrade_.quantity_, 50);

    ASSERT_TRUE(orderBook.isAskEmpty());

    ASSERT_FALSE(orderBook.isBidEmpty());
    auto bid = orderBook.getBestBid();
    ASSERT_EQ(bid->getPrice(), 10.0);
    ASSERT_EQ(bid->getFilledQuantity(), 50);
    ASSERT_EQ(bid->getInitialQuantity(), 100);
    ASSERT_EQ(bid->getOrderId(), "3");
}