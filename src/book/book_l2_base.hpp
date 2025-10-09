#ifndef _BOOK_L2_BASE_HPP
#define _BOOK_L2_BASE_HPP

#include "order.h"

class L2OrderBookBase {
   protected:
    void addOrderImpl(const Order* order);

    void cancelOrder(const Order* order);

    void printImpl();
};

#endif  // _BOOK_L2_BASE_HPP