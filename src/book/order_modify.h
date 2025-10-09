#pragma once

#include "order.h"

class OrderModify {
   public:
    OrderModify(Side side, Price price, Quantity quantity)
        : price_{price}, side_{side}, quantity_{quantity} {}

    Price getPrice() const { return price_; }
    Side getSide() const { return side_; }
    Quantity getQuantity() const { return quantity_; }

    void toOrderPointer(Order *order) const {
        order->price_ = price_;
        order->side_ = side_;
        order->remainingQuantity_ = quantity_;
        order->initialQuantity_ = quantity_;
    }

   private:
    Price price_;
    Side side_;
    Quantity quantity_;
};