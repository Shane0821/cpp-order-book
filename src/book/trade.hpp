#include "order.hpp"

struct Trade {
    Trade(Order::OrderID _buyOrderId, Order::OrderID _sellOrderId, Order::Price _price,
          Order::Quant _quantity)
        : buyOrderId(_buyOrderId),
          sellOrderId(_sellOrderId),
          price(_price),
          quantity(_quantity),
          timestamp(std::chrono::high_resolution_clock::now()) {}

    Order::OrderID buyOrderId;
    Order::OrderID sellOrderId;
    Order::Price price;
    Order::Quant quantity;
    Order::Timestamp timestamp;
};