#ifndef _BOOK_L2_BASE_HPP
#define _BOOK_L2_BASE_HPP

#include "level_info.h"
#include "level_traits.hpp"
#include "order.h"
#include "trade.h"

template <typename Derived>
class L2OrderBook {
   public:
    L2OrderBook() = default;
    virtual ~L2OrderBook() = default;

    void addOrder(const Order* order) {
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

    void cancelOrder(const Order* order) {
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

#endif  // _BOOK_L2_BASE_HPP