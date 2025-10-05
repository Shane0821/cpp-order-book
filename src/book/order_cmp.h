#pragma once

#include "order.h"

struct OrderComparison {
    bool operator()(const Order &a, const Order &b) const {
        return a.creationTime_ < b.creationTime_;
    }

    bool operator()(const Order *a, const Order *b) const {
        return a->creationTime_ < b->creationTime_;
    }
};