#ifndef _BOOK_L3_HPP
#define _BOOK_L3_HPP

#include <concepts>
#include <functional>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "book_base.hpp"
#include "level_searcher.hpp"

template <typename LevelContainer>
class MapBasedL3OrderBook : L3OrderBook<MapBasedL3OrderBook<LevelContainer>> {
   public:
    std::map<Price, LevelContainer, std::greater<Price>> bidLevels_;
    std::map<Price, LevelContainer, std::less<Price>> askLevels_;

    std::unordered_map<Price, typename decltype(bidLevels_)::iterator> priceToBidItMap_;
    std::unordered_map<Price, typename decltype(askLevels_)::iterator> priceToAskItMap_;
    std::unordered_map<OrderId, typename LevelContainer::iterator>
        idToLevelContainerItMap_;

    MapBasedL3OrderBook() = default;
    ~MapBasedL3OrderBook() = default;

    bool addOrderImpl(const Order* order) {
        if (order->price_ <= 0 || order->remainingQuantity_ <= 0) [[unlikely]] {
            return false;
        }

        if (idToLevelContainerItMap_.contains(order->orderId_)) [[unlikely]] {
            return false;
        }

        if (order->side_ == Side::Buy) {
            idToLevelContainerItMap_[order->orderId_] =
                addOrderToMap(bidLevels_, priceToBidItMap_, order);
        } else {
            idToLevelContainerItMap_[order->orderId_] =
                addOrderToMap(askLevels_, priceToAskItMap_, order);
        }

        return true;
    }

    bool cancelOrderImpl(const Order* order) {
        if (!idToLevelContainerItMap_.contains(order->orderId_)) [[unlikely]] {
            return false;
        }

        auto levelContainerIt = idToLevelContainerItMap_[order->orderId_];

        if (order->side == Side::Buy) {
            cancelOrderFromMap(bidLevels_, priceToBidItMap_, levelContainerIt, order);
        } else {
            cancelOrderFromMap(askLevels_, priceToAskItMap_, levelContainerIt, order);
        }
        // erase order ID
        idToLevelContainerItMap_.erase(order->orderId_);

        return true;
    }

    bool modifyOrderImpl(Order* order, Price newPrice, Quantity newQuantity) {
        if (!idToLevelContainerItMap_.contains(order->orderId_)) [[unlikely]] {
            return false;
        }

        // cancel the order
        cancelOrder(order);
        // update the order
        order->initialQuantity_ = newQuantity;
        order->remainingQuantity_ = std::min(order->remainingQuantity_, newQuantity);
        order->price_ = newPrice;
        // add the order
        addOrder(order);
        return true;
    }

    Order* getBestBidImpl() const {
        return LevelContainerTraits<LevelContainer>::first(bidLevels_.begin()->second);
    }

    Order* getBestAskImpl() const {
        return LevelContainerTraits<LevelContainer>::first(askLevels_.begin()->second);
    }

    void printImpl() const {}

   private:
    template <typename T, typename M>
    decltype(auto) addOrderToMap(T& levels, M& price_map, const Order* order) {
        if (price_map.contains(order->price_)) [[likely]] {
            auto& levelContainer = price_map[order->price_]->second;
            return LevelContainerTraits<LevelContainer>::insert(levelContainer, order);
        }
        // create a new level
        auto [it, inserted] = levels.emplace(order.price_, LevelContainer{});
        price_map[order->price_] = it;

        // insert order into the level
        // it->second is the container
        return LevelContainerTraits<LevelContainer>::insert(it->second, order);
    }

    template <typename T, typename M, typename Iterator>
    void cancelOrderFromMap(T& levels, M& price_map, Iterator levelContainerIt,
                            const Order* order) {
        auto levelIt = price_map[order->price_];
        auto& levelContainer = levelIt->second;
        // erase order from the level
        LevelContainerTraits<LevelContainer>::erase(levelContainer, levelContainerIt);

        // erase the level if it's empty
        if (LevelContainerTraits<LevelContainer>::empty(levelContainer)) [[unlikely]] {
            levels.erase(levelIt);
            price_map.erase(order->price_);
        }
    }
};

template <typename LevelContainer, typename LevelSearcher = BinaryLevelSearcher,
          size_t MAX_DEPTH = 65536>
class VectorBasedL3OrderBook
    : public L3OrderBook<VectorBasedOrderBook<LevelContainer, LevelSearcher, MAX_DEPTH>> {
   public:
    using Levels = std::vector<L3LevelInfo<LevelContainer>>;

    Levels bidLevels_;  // largest price at the end
    Levels askLevels_;  // smallest price at the end

    std::unordered_map<OrderId, typename LevelContainer::iterator>
        orderIDToLevelContainerItMap_;

    VectorBasedOrderBook() {
        // reserve memory to avoid runtime reallocation
        bidLevels_.reserve(MAX_DEPTH);
        askLevels_.reserve(MAX_DEPTH);
    }

    ~VectorBasedOrderBook() = default;

    bool addOrderImpl(Order* order) {
        if (order->side_ == Side::Buy) {
            orderIDToLevelContainerItMap_[order->orderId_] =
                levelAddOrder(bidLevels_, order, std::less<Price>{});
        } else {
            orderIDToLevelContainerItMap_[order->orderId_] =
                levelAddOrder(askLevels_, order, std::greater<Price>{});
        }
        return true;
    }

    bool cancelOrderImpl(Order* order) {
        auto it = orderIDToLevelContainerItMap_.find(order->orderId_);
        if (it == orderIDToLevelContainerItMap_.end()) [[unlikely]] {
            return false;
        }

        auto& levelContainerIt = it->second;

        if (order->side_ == Side::Buy) {
            levelRemoveOrder(bidLevels_, levelContainerIt, order, std::less<Price>{});
        } else {
            levelRemoveOrder(askLevels_, levelContainerIt, order, std::greater<Price>{});
        }

        // erase order ID from the hashmap
        orderIDToLevelContainerItMap_.erase(order->orderId_);
        return true;
    }

    bool modifyOrderImpl(Order* order, Price newPrice, Quantity newQuantity) {
        if (!orderIDToLevelContainerItMap_.contains(order->orderId_)) [[unlikely]] {
            return false;
        }

        // cancel the order
        cancelOrderImpl(order);
        // update the order
        order->initialQuantity_ = newQuantity;
        order->remainingQuantity_ = std::min(order->remainingQuantity_, newQuantity);
        order->price_ = newPrice;
        // add the order
        addOrderImpl(order);
        return true;
    }

    const Order* getBestBidImpl() const {
        return LevelContainerTraits<LevelContainer>::first(
            bidLevels_.rbegin()->levelContainer_);
    }
    const Order* getBestAskImpl() const {
        return LevelContainerTraits<LevelContainer>::first(
            askLevels_.rbegin()->levelContainer_);
    }
    Order* getBestBidImpl() {
        return LevelContainerTraits<LevelContainer>::first(
            bidLevels_.rbegin()->levelContainer_);
    }
    Order* getBestAskImpl() {
        return LevelContainerTraits<LevelContainer>::first(
            askLevels_.rbegin()->levelContainer_);
    }

    void printImpl() const {}

   private:
    template <typename T, typename Compare>
    void levelAddOrder(T& levels, Order* order, Compare cmp) {
        auto it = LevelSearcher::findLevelIt(levels, order->price_, cmp);

        if (it != levels.end() && it->price_ == order->price_) [[likely]] {
            // price level exists, insert order into the level
            return LevelContainerTraits<LevelContainer>::insert(it->levelContainer_,
                                                                order);
        }

        // price level doesn't exist, create a new level
        auto vecIt = levels.insert(it, L3LevelInfo{order->price_, LevelContainer{}});
        return LevelContainerTraits<LevelContainer>::insert(vecIt->levelContainer_,
                                                            order);
    }

    template <typanem T, typename Iterator, typename Compare>
    void levelRemoveOrder(T& levels, Iterator levelContainerIt, Order* order,
                          Compare cmp) {
        auto vecIt = LevelSearcher::findLevelIt(levels, order->price_, cmp);
        auto& levelContainer = vecIt->levelContainer_;

        // erase order from the level
        LevelContainerTraits<LevelContainer>::erase(levelContainer, levelContainerIt);

        // erase the level if it's empty
        if (LevelContainerTraits<LevelContainer>::empty(levelContainer)) [[unlikely]] {
            // erase the level from the vector
            levels.erase(vecIt);
        }
    }
};

#endif  // _BOOK_L3_HPP