#ifndef _BOOK_L2_HPP
#define _BOOK_L2_HPP

#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "book_base.hpp"
#include "level_searcher.hpp"

class MapBasedL2OrderBook : public L2OrderBook<MapBasedL2OrderBook> {
   public:
    std::map<Price, L2LevelInfo, std::greater<Price>> bidLevels_;
    std::map<Price, L2LevelInfo, std::less<Price>> askLevels_;

    std::unordered_map<Price, decltype(bidLevels_)::iterator> price2BidLevelIterMap_;
    std::unordered_map<Price, decltype(askLevels_)::iterator> price2AskLevelIterMap_;

    MapBasedL2OrderBook() = default;
    ~MapBasedL2OrderBook() = default;

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

   private:
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
};

template <typename LevelSearcher = BinaryLevelSearcher, size_t MAX_DEPTH = 65536>
class VectorBasedL2OrderBook
    : public L2OrderBook<VectorBasedL2OrderBook<LevelSearcher, MAX_DEPTH>> {
   public:
    using LevelInfos = std::vector<L2LevelInfo>;

    LevelInfos bidLevels_;  // largest price at the end
    LevelInfos askLevels_;  // smallest price at the end

    VectorBasedL2OrderBook() {
        // reserve memory to avoid runtime reallocation
        bidLevels_.reserve(MAX_DEPTH);
        askLevels_.reserve(MAX_DEPTH);
    }

    ~VectorBasedL2OrderBook() = default;

    void addOrderImpl(const Order* order) {
        if (order->side_ == Side::Buy) {
            levelAddOrder(bidLevels_, order->price_, order->remainingQuantity_,
                          std::less<Price>{});
        } else {
            levelAddOrder(askLevels_, order->price_, order->remainingQuantity_,
                          std::greater<Price>{});
        }
    }

    void cancelOrderImpl(const Order* order) {
        if (order->side_ == Side::Buy) {
            levelRemoveOrder(bidLevels_, order->price_, order->remainingQuantity_,
                             std::less<Price>{});
        } else {
            levelRemoveOrder(askLevels_, order->price_, order->remainingQuantity_,
                             std::greater<Price>{});
        }
    }

    void printImpl() const {}

   private:
    template <typename T, typename Compare>
    void levelRemoveOrder(T& levels, Price price, Quantity quantity, Compare cmp) {
        auto it = LevelSearcher::findLevelIt(levels, price, cmp);

        if (it == levels.end() || it->price_ != price) [[unlikely]] {
            return;
        }

        auto& info = *it;
        info.quantity_ -= quantity;
        info.volume_ -= price * quantity;

        // erase the level if it's empty
        if (info.quantity_ <= 0) [[unlikely]] {
            levels.erase(it);
        }
    }

    template <typename T, typename Compare>
    void levelAddOrder(T& levels, Price price, Quantity quantity, Compare cmp) {
        auto it = LevelSearcher::findLevelIt(levels, price, cmp);

        if (it != levels.end() && it->price_ == price) [[likely]] {
            // level exists
            it->quantity_ += quantity;
            it->volume_ += price * quantity;
        } else {
            // level does not exist
            levels.insert(it, L2LevelInfo{price, quantity, price * quantity});
        }
    }
};

#endif  // _BOOK_L2_HPP