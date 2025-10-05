#ifndef _BOOK_BASE_HPP
#define _BOOK_BASE_HPP

#include <concepts>
#include <functional>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "level_info.h"
#include "level_traits.hpp"
#include "order.h"
#include "trade.h"

template <typename Derived>
class L2OrderBook {
   public:
    L2OrderBook() = default;
    virtual ~L2OrderBook() = default;

    void addOrder(Order* order) {
        // type check
        static_assert(std::is_same_v<decltype(order->price_), Price>);
        static_assert(std::is_same_v<decltype(order->initialQuantity_), Quantity>);
        static_assert(std::is_same_v<decltype(order->remainingQuantity_), Quantity>);
        static_assert(std::is_same_v<decltype(order->orderId_), OrderId>);

        if (order == nullptr || order->remainingQuantity_ == 0) [[unlikely]] {
            return;
        }

        static_cast<Derived*>(this)->addOrderImpl(order);
    }

    void cancelOrder(Order* order) {
        if (order == nullptr) [[unlikely]] {
            return;
        }
        
        static_cast<Derived*>(this)->cancelOrderImpl(order);
    }

    bool isBidEmpty() const {
        return static_cast<const Derived*>(this)->bidLevels_.empty();
    }
    bool isAskEmpty() const {
        return static_cast<const Derived*>(this)->askLevels_.empty();
    }

    void print() const { static_cast<const Derived*>(this)->printImpl(); }
};

template <typename Derived>
class L3OrderBook {
   public:
    L3OrderBook() = default;
    virtual ~L3OrderBook() = default;

    // add order to the order book
    bool addOrder(Order* order) {
        // type check
        static_assert(std::is_same_v<decltype(order->price_), Price>);
        static_assert(std::is_same_v<decltype(order->initialQuantity_), Quantity>);
        static_assert(std::is_same_v<decltype(order->remainingQuantity_), Quantity>);
        static_assert(std::is_same_v<decltype(order->orderId_), OrderId>);

        if (order == nullptr || order->remainingQuantity_ == 0) [[unlikely]] {
            return false;
        }

        return static_cast<Derived*>(this)->addOrderImpl(order);
    }

    // cancel order by order ID
    bool cancelOrder(Order* order) {
        if (order == nullptr) [[unlikely]] {
            return false;
        }

        return static_cast<Derived*>(this)->cancelOrderImpl(order);
    }

    // modify order quantity and price
    bool modifyOrder(Order* order, Price newPrice, Quantity newQuantity) {
        if (order == nullptr) [[unlikely]] {
            return false;
        }

        return static_cast<Derived*>(this)->modifyOrderImpl(order, newPrice, newQuantity);
    }

    bool isBidEmpty() const {
        return static_cast<const Derived*>(this)->bidLevels_.empty();
    }

    bool isAskEmpty() const {
        return static_cast<const Derived*>(this)->askLevels_.empty();
    }

    const Order* getBestBid() const {
        return static_cast<const Derived*>(this)->getBestBidImpl();
    }
    const Order* getBestAsk() const {
        return static_cast<const Derived*>(this)->getBestAskImpl();
    }
    Order* getBestBid() { return static_cast<Derived*>(this)->getBestBidImpl(); }
    Order* getBestAsk() { return static_cast<Derived*>(this)->getBestAskImpl(); }

    // print the order book
    void print() const { static_cast<const Derived*>(this)->printImpl(); }
};

#endif  // _BOOK_BASE_HPP