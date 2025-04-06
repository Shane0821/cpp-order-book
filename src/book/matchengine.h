#ifndef _MATCHENGINE_HPP
#define _MATCHENGINE_HPP

#include "orderbook.hpp"
#include "trade.hpp"

class MatchingEngine {
   public:
    using MatchEventHandler = std::function<void(const Trade&)>;

    bool addOrder(const Order& order);
    bool cancelOrder(Order::OrderID orderId);
    bool modifyOrder(Order::OrderID orderId, Order::Quant newQty, Order::Price newPrice);

    void registerMatchEvent(MatchEventHandler handler);

   private:
    void execute();

    MatchEventHandler onMatch_;
    MapBasedPriceLevelOrderBook<std::list<Order>> orderBook_;
};

#endif  // _MATCHENGINE_HPP
