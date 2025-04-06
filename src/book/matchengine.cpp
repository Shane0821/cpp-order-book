#include "matchengine.h"

bool MatchingEngine::addOrder(const Order& order) {
    auto ret = orderBook_.addOrder(order);
    if (ret) execute();
    return ret;
}

bool MatchingEngine::cancelOrder(Order::OrderID orderId) {
    return orderBook_.cancelOrder(orderId);
}

bool MatchingEngine::modifyOrder(Order::OrderID orderId, Order::Quant newQty,
                                 Order::Price newPrice) {
    auto ret = orderBook_.modifyOrder(orderId, newQty, newPrice);
    if (ret) execute();
    return ret;
}

void MatchingEngine::registerMatchEvent(MatchEventHandler handler) {
    onMatch_ = std::move(handler);
}

void MatchingEngine::execute() {
    while (!orderBook_.isAskEmpty() && !orderBook_.isBidEmpty()) {
        decltype(auto) ask = orderBook_.getBestAsk();
        decltype(auto) bid = orderBook_.getBestBid();

        if (ask.price <= bid.price) {
            auto tradeQuant = std::min(bid.quantity, ask.quantity);
            Trade trade{bid.orderID, ask.orderID, ask.price, tradeQuant};
            if (onMatch_) onMatch_(trade);
            if (tradeQuant == bid.quantity) {
                orderBook_.cancelOrder(bid.orderID);
            } else {
                orderBook_.modifyOrder(bid.orderID, bid.quantity - tradeQuant, bid.price);
            }

            if (tradeQuant == ask.quantity) {
                orderBook_.cancelOrder(ask.orderID);
            } else {
                orderBook_.modifyOrder(ask.orderID, ask.quantity - tradeQuant, ask.price);
            }
        } else {
            // No match possible
            break;
        }
    }
}