#ifndef _BOOK_L2_HPP
#define _BOOK_L2_HPP

#include "book_l2_base.hpp"
#include "level_info.h"
#include "level_traits.hpp"
#include "order.h"
#include "order_validate.h"
#include "trade.h"

template <typename Derived, typename Base = L2OrderBookBase>
class L2OrderBook : public Base {
   public:
    void addOrder(const Order* order) {
        if (!OrderValidate::validte(order)) [[unlikely]] {
            return;
        }

        derived()->addOrderImpl(order);
    }

    void cancelOrder(const Order* order) {
        if (!OrderValidate::validte(order)) [[unlikely]] {
            return;
        }

        derived()->cancelOrderImpl(order);
    }

    void cancelOrder(Side side, Price price, Quantity quantity) {
        if (price <= 0 || quantity <= 0) [[unlikely]] {
            return;
        }

        derived()->cancelOrderImpl(side, price, quantity);
    }

    const auto& getBidLevels() const { return derived()->bidLevels_; }
    const auto& getAskLevels() const { return derived()->askLevels_; }

    bool isBidEmpty() const { return derived()->bidLevels_.empty(); }
    bool isAskEmpty() const { return derived()->askLevels_.empty(); }

    void forEachBidLevel(Price pmin, Price pmax,
                         const std::function<bool(const L2LevelInfo&)>& cb) {
        for (auto it = derived()->bidBegin(); it != derived()->bidEnd(); ++it) {
            const auto& [price, info] = *it;
            if (price > pmax) continue;
            if (price < pmin) break;
            if (!cb(info)) break;
        }
    }

    void forEachAskLevel(Price pmin, Price pmax,
                         const std::function<bool(const L2LevelInfo&)>& cb) {
        for (auto it = derived()->bidBegin(); it != derived()->bidEnd(); ++it) {
            const auto& [price, info] = *it;
            if (price < pmin) continue;
            if (price > pmax) break;
            if (!cb(info)) break;
        }
    }

    void print() const { derived()->printImpl(); }

   protected:
    L2OrderBook() = default;
    ~L2OrderBook() = default;

    Derived* derived() { return static_cast<Derived*>(this); }
    const Derived* derived() const { return static_cast<const Derived*>(this); }
};

#endif  // _BOOK_L2_HPP