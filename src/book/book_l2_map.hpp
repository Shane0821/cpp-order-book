#ifndef _BOOK_L2_MAP_HPP
#define _BOOK_L2_MAP_HPP

#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "book_l2.hpp"
#include "level_searcher.hpp"

class MapBasedL2OrderBook : public L2OrderBook<MapBasedL2OrderBook> {
    friend class L2OrderBook<MapBasedL2OrderBook>;

   protected:
    void addOrderImpl(const Order* order) {
        if (order->side_ == Side::Buy) {
            levelAddOrder(price2BidLevelIterMap_, bidLevels_, order->price_,
                          order->remainingQuantity_);
        } else {
            levelAddOrder(price2AskLevelIterMap_, askLevels_, order->price_,
                          order->remainingQuantity_);
        }
    }

    void cancelOrderImpl(const Order* order) {
        if (order->side_ == Side::Buy) {
            levelRemoveOrder(price2BidLevelIterMap_, bidLevels_, order->price_,
                             order->remainingQuantity_);
        } else {
            levelRemoveOrder(price2AskLevelIterMap_, askLevels_, order->price_,
                             order->remainingQuantity_);
        }
    }

    void printImpl() const {}

   protected:
    template <typename M, typename T>
    void levelAddOrder(M& price2levelItMap, T& levels, Price price, Quantity quantity) {
        // create a new level if it doesn't exist
        if (price2levelItMap.contains(price)) [[likely]] {
            auto& info = price2levelItMap[price]->second;
            info.quantity_ += quantity;
            info.volume_ += price * quantity;
        } else {
            auto [it, inserted] =
                levels.emplace(price, L2LevelInfo{price, quantity, price * quantity});
            price2levelItMap[price] = it;
        }
    }

    template <typename M, typename T>
    void levelRemoveOrder(M& price2levelItMap, T& levels, Price price,
                          Quantity quantity) {
        if (!price2levelItMap.contains(price)) [[unlikely]] {
            return;
        }

        auto it = price2levelItMap[price];
        auto& info = it->second;
        info.quantity_ -= quantity;
        info.volume_ -= price * quantity;

        if (info.quantity_ <= 0) [[unlikely]] {
            levels.erase(it);
            price2levelItMap.erase(price);
        }
    }

    std::map<Price, L2LevelInfo, std::greater<Price>> bidLevels_;
    std::map<Price, L2LevelInfo, std::less<Price>> askLevels_;

    std::unordered_map<Price, decltype(bidLevels_)::iterator> price2BidLevelIterMap_;
    std::unordered_map<Price, decltype(askLevels_)::iterator> price2AskLevelIterMap_;
};

#endif  // _BOOK_L2_MAP_HPP