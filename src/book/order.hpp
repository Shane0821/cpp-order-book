#ifndef _ORDER_HPP
#define _ORDER_HPP

#include <chrono>
#include <iostream>
#include <string>

#include "util/flyweightstring.h"

class Order {
   public:
    enum class Side { Buy, Sell };
    enum class Type { Limit, Market };  // limit order or market order

    using Price = double;
    using Volume = double;
    using Quant = std::uint64_t;
    using OrderID = std::string;
    using Time = std::chrono::time_point<std::chrono::high_resolution_clock>;

    struct Hash {
        std::size_t operator()(const Order& order) const {
            return std::hash<OrderID>()(order.orderID);
        }
    };

    struct Equal {
        bool operator()(const Order& lhs, const Order& rhs) const {
            return lhs.orderID == rhs.orderID;
        }
    };

    explicit Order(OrderID _orderID, std::string _owner, std::string _target,
                   std::string _symbol, Side _side, Quant _quantity, Price _price)
        : orderID(_orderID),
          symbol(std::move(_symbol)),
          owner(std::move(_owner)),
          target(std::move(_target)),
          side(_side),
          quantity(_quantity),
          price(_price),
          creationTime(std::chrono::high_resolution_clock::now()) {}

    friend std::ostream& operator<<(std::ostream& os, const Order& order) {
        return os << "OrderID: " << order.orderID << ", Symbol: " << order.symbol
                  << ", Owner: " << order.owner << ", Target: " << order.target
                  << ", Side: " << (order.side == Side::Buy ? "Buy" : "Sell")
                  << ", Quantity: " << order.quantity << ", Price: " << order.price
                  << std::endl;
    }

    bool operator==(const Order& other) const { return orderID == other.orderID; }
    bool operator<(const Order& other) const {
        if (price == other.price) {
            return creationTime < other.creationTime;
        }
        return price < other.price;
    }
    bool operator>(const Order& other) const {
        if (price == other.price) {
            return creationTime < other.creationTime;
        }
        return price > other.price;
    }

    FlyweightString owner;
    FlyweightString target;
    FlyweightString symbol;
    Side side;
    OrderID orderID;
    Quant quantity{0};
    Price price{0};

    Time creationTime;
};

#endif  // _ORDER_HPP