#ifndef _ORDER_HPP
#define _ORDER_HPP

#include <string>

#include "flyweightstring.hpp"

class Order {
   public:
    enum class Side { Buy, Sell };
    enum class Type { Limit, Market };  // limit order or market order

    using Price = double;
    using Volume = double;
    using Quant = std::uint64_t;
    using OrderID = std::uint64_t;

    explicit Order(OrderID _orderID, std::string _owner, std::string _target,
                   std::string _symbol, Side _side, Quant _quantity, Price _price)
        : orderID(_orderID),
          symbol(std::move(_symbol)),
          owner(std::move(_owner)),
          target(std::move(_target)),
          side(_side),
          quantity(_quantity),
          price(_price),
          openQuantity(_quantity) {}

    FlyweightString owner;
    FlyweightString target;
    FlyweightString symbol;
    Side side;
    OrderID orderID;
    Quant quantity;
    Price price;

    bool canceled{false};
    Quant openQuantity;
    Quant executedQuantity;
    Quant lastExecutedQuantity;
    Price averageExecutedPrice;
    Price lastExecutedPrice;
};

#endif  // _ORDER_HPP