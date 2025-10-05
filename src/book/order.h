#ifndef _ORDER_H
#define _ORDER_H

#include "constants.h"
#include "order_type.h"
#include "side.h"
#include "usings.h"

struct Order {
   public:
    Order() = default;

    Order(OrderId orderID, OrderType type, Side side, Price price, Quantity quantity)
        : orderId_(orderID),
          side_(side),
          type_(type),
          initialQuantity_(quantity),
          remainingQuantity_(quantity),
          price_(price),
          creationTime_(std::chrono::high_resolution_clock::now()) {}

    Order(OrderId orderId, Side side, Quantity quantity)
        : Order(orderId, OrderType::Market, side, Constants::InvalidPrice, quantity) {}

    friend std::ostream& operator<<(std::ostream& os, const Order& order) {
        return os << "OrderID: " << order.orderId_
                  << ", Side: " << (order.side_ == Side::Buy ? "Buy" : "Sell")
                  << ", Initial Quantity: " << order.initialQuantity_
                  << ", Remaining Quantity: " << order.remainingQuantity_
                  << ", Price: " << order.price_ << std::endl;
    }

    bool operator==(const Order& other) const { return orderId_ == other.orderId_; }
    bool operator!=(const Order& other) const { return !(*this == other); }

    Side side_;
    OrderType type_;
    OrderId orderId_;
    Quantity initialQuantity_{0};
    Quantity remainingQuantity_{0};
    Price price_{0};
    Timestamp creationTime_{};
};

#endif  // _ORDER_H