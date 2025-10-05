#include "book/book_l2.hpp"

#include <gtest/gtest.h>

class MapBasedL2OrderBookTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Create test orders
        buyOrder100 = Order{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 1000};
        buyOrder101 = Order{"2", OrderType::GoodTillCancel, Side::Buy, 101.0, 50};
        buyOrder99 = Order{"3", OrderType::GoodTillCancel, Side::Buy, 99, 80};
        sellOrder102 = Order{"4", OrderType::GoodTillCancel, Side::Sell, 102, 1200};
        sellOrder103 = Order{"5", OrderType::GoodTillCancel, Side::Sell, 103, 60};
        sellOrder101 = Order{"6", OrderType::GoodTillCancel, Side::Sell, 101, 30};
    }

    Order buyOrder100, buyOrder101, buyOrder99, sellOrder102, sellOrder103, sellOrder101;
};

// Test basic order addition for MapBasedL2OrderBook
TEST_F(MapBasedL2OrderBookTest, AddOrder) {
    MapBasedL2OrderBook book;

    // Add buy orders
    book.addOrder(&buyOrder100);
    book.addOrder(&buyOrder101);
    book.addOrder(&buyOrder99);

    // Add sell orders
    book.addOrder(&sellOrder102);
    book.addOrder(&sellOrder103);
    book.addOrder(&sellOrder101);

    // Verify bid levels are sorted in descending order
    ASSERT_FALSE(book.bidLevels_.empty());
    auto bidIt = book.bidLevels_.begin();
    ASSERT_EQ(bidIt->first, 101);  // Highest bid first
    bidIt++;
    ASSERT_EQ(bidIt->first, 100);
    bidIt++;
    ASSERT_EQ(bidIt->first, 99);

    // Verify ask levels are sorted in ascending order
    ASSERT_FALSE(book.askLevels_.empty());
    auto askIt = book.askLevels_.begin();
    ASSERT_EQ(askIt->first, 101);  // Lowest ask first
    askIt++;
    ASSERT_EQ(askIt->first, 102);
    askIt++;
    ASSERT_EQ(askIt->first, 103);
}

// Test order cancellation for MapBasedL2OrderBook
TEST_F(MapBasedL2OrderBookTest, CancelOrder) {
    MapBasedL2OrderBook book;

    // Add orders
    book.addOrder(&buyOrder100);
    book.addOrder(&buyOrder101);
    book.addOrder(&sellOrder102);

    // Verify initial state
    ASSERT_EQ(book.bidLevels_.size(), 2);
    ASSERT_EQ(book.askLevels_.size(), 1);

    // Cancel buy order
    Order cancelBuy100 = buyOrder100;
    cancelBuy100.remainingQuantity_ = 500;  // Cancel half
    book.cancelOrder(&cancelBuy100);

    // Verify partial cancellation
    auto bidIt = book.bidLevels_.find(100);
    ASSERT_NE(bidIt, book.bidLevels_.end());
    ASSERT_EQ(bidIt->second.quantity_, 500);  // Remaining quantity

    // Cancel entire order
    Order fullCancelBuy100 = buyOrder100;
    fullCancelBuy100.remainingQuantity_ = 1000;
    book.cancelOrder(&fullCancelBuy100);

    // Verify level removed when quantity reaches 0
    bidIt = book.bidLevels_.find(100);
    ASSERT_EQ(bidIt, book.bidLevels_.end());
    ASSERT_EQ(book.bidLevels_.size(), 1);
}

// Test quantity aggregation at same price level for MapBasedL2OrderBook
TEST_F(MapBasedL2OrderBookTest, AggregateSamePrice) {
    MapBasedL2OrderBook book;

    // Add multiple orders at same price
    Order buyOrder100_1 = Order{"1", OrderType::GoodTillCancel, Side::Buy, 100, 1000};
    Order buyOrder100_2 = Order{"2", OrderType::GoodTillCancel, Side::Buy, 100, 500};

    book.addOrder(&buyOrder100_1);
    book.addOrder(&buyOrder100_2);

    // Verify aggregation
    ASSERT_EQ(book.bidLevels_.size(), 1);
    auto bidIt = book.bidLevels_.find(100);
    ASSERT_NE(bidIt, book.bidLevels_.end());
    ASSERT_EQ(bidIt->second.quantity_, 1500);
    ASSERT_EQ(bidIt->second.volume_, 1500 * 100);
}

// Test empty book detection
TEST_F(MapBasedL2OrderBookTest, EmptyBook) {
    MapBasedL2OrderBook book;

    ASSERT_TRUE(book.isBidEmpty());
    ASSERT_TRUE(book.isAskEmpty());

    book.addOrder(&buyOrder100);
    ASSERT_FALSE(book.isBidEmpty());
    ASSERT_TRUE(book.isAskEmpty());

    book.addOrder(&sellOrder102);
    ASSERT_FALSE(book.isBidEmpty());
    ASSERT_FALSE(book.isAskEmpty());

    Order cancelBuy100 = buyOrder100;
    cancelBuy100.remainingQuantity_ = 1000;
    book.cancelOrder(&cancelBuy100);
    ASSERT_TRUE(book.isBidEmpty());
    ASSERT_FALSE(book.isAskEmpty());
}

// Test edge cases
TEST_F(MapBasedL2OrderBookTest, EdgeCases) {
    MapBasedL2OrderBook book;

    // Test cancelling non-existent order
    Order nonExistentOrder = Order{"999", OrderType::GoodTillCancel, Side::Buy, 999, 100};
    book.cancelOrder(&nonExistentOrder);  // Should not crash

    // Test adding order with zero quantity
    Order zeroQuantityOrder = Order{"999", OrderType::GoodTillCancel, Side::Buy, 100, 0};
    book.addOrder(&zeroQuantityOrder);

    // Verify book is still empty
    ASSERT_TRUE(book.isBidEmpty());
}

// Test cross-book operations (adding and cancelling in mixed order)
TEST_F(MapBasedL2OrderBookTest, MixedOperations) {
    MapBasedL2OrderBook book;

    std::vector<Order> orders = {
        Order{"1", OrderType::GoodTillCancel, Side::Buy, 100, 1000},
        Order{"2", OrderType::GoodTillCancel, Side::Sell, 102, 500},
        Order{"3", OrderType::GoodTillCancel, Side::Buy, 101, 800},
        Order{"4", OrderType::GoodTillCancel, Side::Sell, 101, 300},
        Order{"5", OrderType::GoodTillCancel, Side::Buy, 99, 600},
        Order{"6", OrderType::GoodTillCancel, Side::Sell, 103, 400}};

    // Add all orders
    for (auto& order : orders) {
        book.addOrder(&order);
    }

    ASSERT_EQ(book.bidLevels_.size(), 3);
    ASSERT_EQ(book.askLevels_.size(), 3);

    // Cancel some orders in mixed order
    std::vector<Order> cancels = {
        Order{"3", OrderType::GoodTillCancel, Side::Buy, 101,
              400},  // Cancel half of order 3
        Order{"2", OrderType::GoodTillCancel, Side::Sell, 102,
              500},  // Cancel all of order 2
        Order{"5", OrderType::GoodTillCancel, Side::Buy, 99, 600}
        // Cancel all of order 5
    };

    for (auto& cancel : cancels) {
        book.cancelOrder(&cancel);
    }

    ASSERT_EQ(book.bidLevels_.size(), 2);  // 100, 101(partial)
    ASSERT_EQ(book.askLevels_.size(), 2);  // 101, 103

    // Verify remaining quantities
    auto bid100It = book.bidLevels_.find(100);
    ASSERT_NE(bid100It, book.bidLevels_.end());
    ASSERT_EQ(bid100It->second.quantity_, 1000);

    auto bid101It = book.bidLevels_.find(101);
    ASSERT_NE(bid101It, book.bidLevels_.end());
    ASSERT_EQ(bid101It->second.quantity_, 400);  // 800 - 400

    auto ask101It = book.askLevels_.find(101);
    ASSERT_NE(ask101It, book.askLevels_.end());
    ASSERT_EQ(ask101It->second.quantity_, 300);
}

// Template test for all VectorBased implementations
template <typename T>
class VectorBasedL2OrderBookTest : public ::testing::Test {
   protected:
    void SetUp() override {
        buyOrder100 = Order{"1", OrderType::GoodTillCancel, Side::Buy, 100.0, 1000};
        buyOrder101 = Order{"2", OrderType::GoodTillCancel, Side::Buy, 101.0, 50};
        buyOrder99 = Order{"3", OrderType::GoodTillCancel, Side::Buy, 99, 80};
        sellOrder102 = Order{"4", OrderType::GoodTillCancel, Side::Sell, 102, 1200};
        sellOrder103 = Order{"5", OrderType::GoodTillCancel, Side::Sell, 103, 60};
        sellOrder101 = Order{"6", OrderType::GoodTillCancel, Side::Sell, 101, 30};
    }

    Order buyOrder100, buyOrder101, buyOrder99, sellOrder102, sellOrder103, sellOrder101;
};

using VectorBasedTypes =
    ::testing::Types<VectorBasedL2OrderBook<BinaryLevelSearcher>,
                     VectorBasedL2OrderBook<BranchlessBinaryLevelSearcher>,
                     VectorBasedL2OrderBook<LinearLevelSearcher> >;

TYPED_TEST_SUITE(VectorBasedL2OrderBookTest, VectorBasedTypes);

// Test basic functionality for all VectorBased implementations
TYPED_TEST(VectorBasedL2OrderBookTest, AddOrder) {
    TypeParam book;

    // Add buy orders
    book.addOrder(&this->buyOrder100);
    book.addOrder(&this->buyOrder101);
    book.addOrder(&this->buyOrder99);

    // Add sell orders
    book.addOrder(&this->sellOrder102);
    book.addOrder(&this->sellOrder103);
    book.addOrder(&this->sellOrder101);

    // Verify bid levels are sorted in descending order (largest at end)
    ASSERT_FALSE(book.bidLevels_.empty());
    ASSERT_EQ(book.bidLevels_.back().price_, 101);  // Highest bid at end
    ASSERT_EQ(book.bidLevels_.front().price_, 99);  // Lowest bid at front

    // Verify ask levels are sorted in ascending order (smallest at end)
    ASSERT_FALSE(book.askLevels_.empty());
    ASSERT_EQ(book.askLevels_.back().price_, 101);   // Lowest ask at end
    ASSERT_EQ(book.askLevels_.front().price_, 103);  // Highest ask at front
}

TYPED_TEST(VectorBasedL2OrderBookTest, CancelOrder) {
    TypeParam book;

    // Add orders
    book.addOrder(&this->buyOrder100);
    book.addOrder(&this->buyOrder101);
    book.addOrder(&this->sellOrder102);

    // Verify initial state
    ASSERT_EQ(book.bidLevels_.size(), 2);
    ASSERT_EQ(book.askLevels_.size(), 1);

    // Cancel partial buy order
    Order cancelBuy100 = this->buyOrder100;
    cancelBuy100.remainingQuantity_ = 500;
    book.cancelOrder(&cancelBuy100);

    // Verify partial cancellation
    bool found100 = false;
    for (const auto& level : book.bidLevels_) {
        if (level.price_ == 100) {
            found100 = true;
            ASSERT_EQ(level.quantity_, 500);
            break;
        }
    }
    ASSERT_TRUE(found100);

    // Cancel entire order
    Order fullCancelBuy100 = this->buyOrder100;
    fullCancelBuy100.remainingQuantity_ = 1000;
    book.cancelOrder(&fullCancelBuy100);

    // Verify level removed
    found100 = false;
    for (const auto& level : book.bidLevels_) {
        if (level.price_ == 100) {
            found100 = true;
            break;
        }
    }
    ASSERT_FALSE(found100);
    ASSERT_EQ(book.bidLevels_.size(), 1);
}

TYPED_TEST(VectorBasedL2OrderBookTest, AggregateSamePrice) {
    TypeParam book;

    // Add multiple orders at same price
    Order buyOrder100_1 = Order{"1", OrderType::GoodTillCancel, Side::Buy, 100, 1000};
    Order buyOrder100_2 = Order{"2", OrderType::GoodTillCancel, Side::Buy, 100, 500};

    book.addOrder(&buyOrder100_1);
    book.addOrder(&buyOrder100_2);

    // Verify aggregation
    ASSERT_EQ(book.bidLevels_.size(), 1);
    ASSERT_EQ(book.bidLevels_[0].price_, 100);
    ASSERT_EQ(book.bidLevels_[0].quantity_, 1500);
    ASSERT_EQ(book.bidLevels_[0].volume_, 1500 * 100);
}

TYPED_TEST(VectorBasedL2OrderBookTest, EmptyBook) {
    TypeParam book;

    ASSERT_TRUE(book.isBidEmpty());
    ASSERT_TRUE(book.isAskEmpty());

    book.addOrder(&this->buyOrder100);
    ASSERT_FALSE(book.isBidEmpty());
    ASSERT_TRUE(book.isAskEmpty());

    book.addOrder(&this->sellOrder102);
    ASSERT_FALSE(book.isBidEmpty());
    ASSERT_FALSE(book.isAskEmpty());

    Order cancelBuy100 = this->buyOrder100;
    cancelBuy100.remainingQuantity_ = 1000;
    book.cancelOrder(&cancelBuy100);
    ASSERT_TRUE(book.isBidEmpty());
    ASSERT_FALSE(book.isAskEmpty());
}

TYPED_TEST(VectorBasedL2OrderBookTest, ManyLevels) {
    TypeParam book;
    const int NUM_LEVELS = 1000;

    // Add many bid levels
    for (int i = 1; i <= NUM_LEVELS; ++i) {
        Order order =
            Order{"", OrderType::GoodTillCancel, Side::Buy, (Price)i, (Quantity)i * 10};
        book.addOrder(&order);
    }

    // Add many ask levels
    for (int i = NUM_LEVELS + 1; i <= NUM_LEVELS * 2; ++i) {
        Order order =
            Order{"", OrderType::GoodTillCancel, Side::Sell, (Price)i, (Quantity)i * 10};
        book.addOrder(&order);
    }

    ASSERT_EQ(book.bidLevels_.size(), NUM_LEVELS);
    ASSERT_EQ(book.askLevels_.size(), NUM_LEVELS);

    // Verify sorting
    for (size_t i = 1; i < book.bidLevels_.size(); ++i) {
        ASSERT_LT(book.bidLevels_[i - 1].price_, book.bidLevels_[i].price_);
    }

    for (size_t i = 1; i < book.askLevels_.size(); ++i) {
        ASSERT_GT(book.askLevels_[i - 1].price_, book.askLevels_[i].price_);
    }
}
