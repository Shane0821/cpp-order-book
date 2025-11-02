#pragma once

#include <functional>

#include "order.h"

struct OrderCompare {
    bool operator()(const Order &a, const Order &b) const {
        return a.creationTime_ < b.creationTime_;
    }

    bool operator()(const Order *a, const Order *b) const {
        return a->creationTime_ < b->creationTime_;
    }
};

struct OrderEqual {
    bool operator()(const Order &a, const Order &b) const { return a == b; }

    bool operator()(const Order *a, const Order *b) const { return *a == *b; }
};

struct OrderHash {
    size_t operator()(const Order &a) const {
        return std::hash<std::string>{}(a.orderId_);
    }
    size_t operator()(const Order *a) const {
        return std::hash<std::string>{}(a->orderId_);
    }
};