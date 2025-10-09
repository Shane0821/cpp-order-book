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

    const OrderId& getOrderId() const { return orderId_; }
    Side getSide() const { return side_; }
    Price getPrice() const { return price_; }
    OrderType getOrderType() const { return type_; }
    Quantity getInitialQuantity() const { return initialQuantity_; }
    Quantity getRemainingQuantity() const { return remainingQuantity_; }
    Quantity getFilledQuantity() const {
        return getInitialQuantity() - getRemainingQuantity();
    }

    bool isFilled() const { return getRemainingQuantity() == 0; }

    bool fill(Quantity quantity) {
        if (quantity > getRemainingQuantity()) [[unlikely]] {
            return false;
        }

        remainingQuantity_ -= quantity;
        return true;
    }

    bool toGoodTillCancel(Price price) {
        if (getOrderType() != OrderType::Market) {
            // order cannot have its price adjusted, only market order can
            return false;
        }

        price_ = price;
        type_ = OrderType::GoodTillCancel;
        return true;
    }

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

using OrderPool = ObjectPool<Order>;

#endif  // _ORDER_H