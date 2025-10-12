#pragma

#include "order.h"

struct OrderValidate {
    static bool validte(const Order *order) {
        if (order == nullptr || order->remainingQuantity_ <= 0 || order->price_ <= 0)
            [[unlikely]] {
            return false;
        }
        return true;
    }
};