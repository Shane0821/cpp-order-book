#ifndef _BOOK_L2_HPP
#define _BOOK_L2_HPP

#include "book_l2_base.hpp"
#include "level_info.h"
#include "level_traits.hpp"
#include "order.h"
#include "trade.h"

template <typename Derived, typename Base = L2OrderBookBase>
class L2OrderBook : public Base {
   public:
    L2OrderBook() = default;
    virtual ~L2OrderBook() = default;

    void addOrder(const Order* order) {
        // type check
        static_assert(std::is_same_v<decltype(order->price_), Price>);
        static_assert(std::is_same_v<decltype(order->initialQuantity_), Quantity>);
        static_assert(std::is_same_v<decltype(order->remainingQuantity_), Quantity>);

        if (order == nullptr || order->remainingQuantity_ == 0 || order->price_ < 0)
            [[unlikely]] {
            return;
        }

        static_cast<Derived*>(this)->addOrderImpl(order);
    }

    void cancelOrder(const Order* order) {
        if (order == nullptr || order->price_ < 0) [[unlikely]] {
            return;
        }

        static_cast<Derived*>(this)->cancelOrderImpl(order);
    }

    const auto& getBidLevels() const {
        return static_cast<const Derived*>(this)->bidLevels_;
    }
    const auto& getAskLevels() const {
        return static_cast<const Derived*>(this)->askLevels_;
    }

    bool isBidEmpty() const {
        return static_cast<const Derived*>(this)->bidLevels_.empty();
    }
    bool isAskEmpty() const {
        return static_cast<const Derived*>(this)->askLevels_.empty();
    }

    void print() const { static_cast<const Derived*>(this)->printImpl(); }
};

#endif  // _BOOK_L2_HPP