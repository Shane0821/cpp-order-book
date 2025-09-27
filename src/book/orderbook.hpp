#ifndef _ORDER_BOOK_HPP
#define _ORDER_BOOK_HPP

#include <concepts>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <functional>

#include "booktraits.hpp"

template <typename Derived>
class OrderBook {
   public:
    OrderBook() = default;
    virtual ~OrderBook() = default;

    // add order to the order book
    bool addOrder(const Order& order) {
        // type check
        static_assert(std::is_same_v<decltype(order.price), Order::Price>,
                      "price must be Order::Price type");
        static_assert(std::is_same_v<decltype(order.quantity), Order::Quant>,
                      "quantity must be Order::Quant type");
        static_assert(std::is_same_v<decltype(order.orderID), Order::OrderID>,
                      "orderID must be Order::OrderID type");

        if (order.price <= 0) {
            // error handling
            return false;
        }
        if (order.quantity <= 0) {
            // error handling
            return false;
        }
        return static_cast<Derived*>(this)->addOrderImpl(order);
    }

    // cancel order by order ID
    bool cancelOrder(const Order::OrderID& orderID) {
        return static_cast<Derived*>(this)->cancelOrderImpl(orderID);
    }

    // modify order quantity and price
    bool modifyOrder(const Order::OrderID& orderID, Order::Quant newQuantity,
                     Order::Price newPrice) {
        return static_cast<Derived*>(this)->updateOrderQuantityImpl(orderID, newQuantity,
                                                                    newPrice);
    }

    bool isBidEmpty() const { return static_cast<const Derived*>(this)->bids.empty(); }

    bool isAskEmpty() const { return static_cast<const Derived*>(this)->asks.empty(); }

    // get the best bid order
    const Order& getBestBid() const {
        return static_cast<const Derived*>(this)->getBestBidImpl();
    }

    // get the best ask order
    const Order& getBestAsk() const {
        return static_cast<const Derived*>(this)->getBestAskImpl();
    }

    // print the order book
    void print() const { static_cast<const Derived*>(this)->printImpl(); }
};

template <typename LevelContainer>
class MapBasedPriceLevelOrderBook
    : public OrderBook<MapBasedPriceLevelOrderBook<LevelContainer>> {
   public:
    std::map<Order::Price, LevelContainer, std::greater<Order::Price>> bids;
    std::map<Order::Price, LevelContainer, std::less<Order::Price>> asks;

    std::unordered_map<Order::OrderID, typename decltype(bids)::iterator>
        orderIDToMapItMap;
    std::unordered_map<Order::OrderID, typename LevelContainer::iterator>
        orderIDToLevelContainerItMap;

    MapBasedPriceLevelOrderBook() = default;
    ~MapBasedPriceLevelOrderBook() = default;

    bool addOrderImpl(const Order& order) {
        if (orderIDToMapItMap.find(order.orderID) != orderIDToMapItMap.end()) {
            return false;
        }

        if (order.side == Order::Side::Buy) {
            // add order to the bid side
            return addOrderToMap(bids, order);
        } else {
            // add order to the ask side
            return addOrderToMap(asks, order);
        }

        return true;
    }

    bool cancelOrderImpl(const Order::OrderID& orderID) {
        auto it = orderIDToMapItMap.find(orderID);
        if (it == orderIDToMapItMap.end()) {
            return false;
        }

        auto& mapIt = it->second;
        auto& levelContainer = mapIt->second;
        auto& levelContainerIt = orderIDToLevelContainerItMap[orderID];
        auto side = levelContainerIt->side;

        // erase order from the level
        LevelContainerTraits<LevelContainer>::erase(levelContainer, levelContainerIt);

        // erase the level if it's empty
        if (LevelContainerTraits<LevelContainer>::empty(levelContainer)) {
            side == Order::Side::Buy ? bids.erase(mapIt) : asks.erase(mapIt);
        }

        // erase order ID from the maps
        orderIDToLevelContainerItMap.erase(orderID);
        orderIDToMapItMap.erase(orderID);
        return true;
    }

    bool modifyOrder(const Order::OrderID& orderID, Order::Quant newQuantity,
                     Order::Price newPrice) {
        auto it = orderIDToLevelContainerItMap.find(orderID);
        if (it == orderIDToLevelContainerItMap.end()) {
            return false;
        }

        auto levelContainerIt = it->second;
        auto order = *levelContainerIt;

        // within the same level
        if (order.price == newPrice) {
            order.quantity = newQuantity;
            order.creationTime = std::chrono::high_resolution_clock::now();

            auto& container = orderIDToMapItMap[orderID]->second;

            // erase the old iterator
            LevelContainerTraits<LevelContainer>::erase(container, levelContainerIt);
            // insert the new order
            auto newIt = LevelContainerTraits<LevelContainer>::insert(container, order);

            orderIDToLevelContainerItMap[orderID] = newIt;
            return true;
        }

        // cancel the order
        cancelOrderImpl(orderID);
        // update the order
        order.quantity = newQuantity;
        order.price = newPrice;
        order.creationTime = std::chrono::high_resolution_clock::now();
        // add the order
        addOrderImpl(order);
        return true;
    }

    const Order& getBestBidImpl() const {
        return *LevelContainerTraits<LevelContainer>::first(bids.begin()->second);
    }

    const Order& getBestAskImpl() const {
        return *LevelContainerTraits<LevelContainer>::first(asks.begin()->second);
    }

    void printImpl() const {}

   private:
    template <typename T>
    bool addOrderToMap(T& map, const Order& order) {
        // create a new level if it doesn't exist
        auto [it, inserted] = map.try_emplace(order.price, LevelContainer{});
        orderIDToMapItMap[order.orderID] = it;

        // insert order into the level
        // it->second is the container
        auto levelContainerIt =
            LevelContainerTraits<LevelContainer>::insert(it->second, order);
        orderIDToLevelContainerItMap[order.orderID] = levelContainerIt;
        return true;
    }
};

template <typename LevelContainer, size_t MAX_DEPTH = 65536>
class VectorBasedOrderBook : public OrderBook<VectorBasedOrderBook<LevelContainer>> {
   public:
    using Levels = std::vector<std::pair<Order::Price, LevelContainer>>;

    Levels bids;  // largest price at the end
    Levels asks;  // smallest price at the end

    std::unordered_map<Order::OrderID, typename LevelContainer::iterator>
        orderIDToLevelContainerItMap;

    VectorBasedOrderBook() {
        // reserve memory to avoid runtime reallocation
        bids.reserve(MAX_DEPTH);
        asks.reserve(MAX_DEPTH);
    }

    ~VectorBasedOrderBook() = default;

    bool addOrderImpl(const Order& order) {
        if (order.side == Order::Side::Buy) {
            addOrderToVec(bids, order, std::less<Order::Price>{});
        } else {
            addOrderToVec(asks, order, std::greater<Order::Price>{});
        }
        return true;
    }

    bool cancelOrderImpl(const Order::OrderID& orderID) {
        auto it = orderIDToLevelContainerItMap.find(orderID);
        if (it == orderIDToLevelContainerItMap.end()) {
            return false;
        }

        auto& levelContainerIt = it->second;
        auto side = levelContainerIt->side;
        auto vecIt = side == Order::Side::Buy ? findLevelIt(bids, levelContainerIt->price,
                                                            std::less<Order::Price>{})
                                              : findLevelIt(asks, levelContainerIt->price,
                                                            std::greater<Order::Price>{});

        auto& levelContainer = vecIt->second;

        // erase order from the level
        LevelContainerTraits<LevelContainer>::erase(levelContainer, levelContainerIt);

        // erase the level if it's empty
        if (LevelContainerTraits<LevelContainer>::empty(levelContainer)) {
            // erase the level from the vector
            side == Order::Side::Buy ? bids.erase(vecIt) : asks.erase(vecIt);
        }

        // erase order ID from the maps
        orderIDToLevelContainerItMap.erase(orderID);
        return true;
    }

    bool modifyOrder(const Order::OrderID& orderID, Order::Quant newQuantity,
                     Order::Price newPrice) {
        auto it = orderIDToLevelContainerItMap.find(orderID);
        if (it == orderIDToLevelContainerItMap.end()) {
            return false;
        }

        auto levelContainerIt = it->second;
        auto order = *levelContainerIt;

        // within the same level
        if (order.price == newPrice) {
            order.quantity = newQuantity;
            order.createTime = std::chrono::system_clock::now();

            auto vecIt =
                order.side == Order::Side::Buy
                    ? findLevelIt(bids, order.price, std::less<Order::Price>{})
                    : findLevelIt(asks, order.price, std::greater<Order::Price>{});
            auto& container = vecIt->second;

            // erase the old iterator
            LevelContainerTraits<LevelContainer>::erase(container, levelContainerIt);
            // insert the new order
            auto newIt = LevelContainerTraits<LevelContainer>::insert(container, order);

            orderIDToLevelContainerItMap[orderID] = newIt;
            return true;
        }

        // cancel the order
        cancelOrderImpl(orderID);
        // update the order
        order.quantity = newQuantity;
        order.price = newPrice;
        order.createTime = std::chrono::system_clock::now();
        // add the order
        addOrderImpl(order);
        return true;
    }

    const Order& getBestBidImpl() const {
        return *LevelContainerTraits<LevelContainer>::first(bids.rbegin()->second);
    }

    const Order& getBestAskImpl() const {
        return *LevelContainerTraits<LevelContainer>::first(asks.rbegin()->second);
    }

    void printImpl() const {}

   private:
    template <typename T, typename Compare>
    decltype(auto) findLevelIt(T& levels, Order::Price price, Compare cmp) {
        return std::lower_bound(
            levels.begin(), levels.end(), price,
            [cmp](const auto& level, auto price) { return cmp(level.first, price); });
    }

    template <typename T, typename Compare>
    void addOrderToVec(T& levels, const Order& order, Compare cmp) {
        auto it = findLevelIt(levels, order.price, cmp);

        if (it != levels.end() && it->first == order.price) {
            // price level exists, insert order into the level
            auto& container = it->second;
            auto levelContainerIt =
                LevelContainerTraits<LevelContainer>::insert(container, order);
            orderIDToLevelContainerItMap[order.orderID] = levelContainerIt;
        } else {
            // price level doesn't exist, create a new level
            auto vecIt = levels.insert(it, {order.price, LevelContainer{}});
            auto& container = vecIt->second;
            auto levelContainerIt =
                LevelContainerTraits<LevelContainer>::insert(container, order);
            orderIDToLevelContainerItMap[order.orderID] = levelContainerIt;
        }
    }
};

#endif  // _ORDER_BOOK_HPP