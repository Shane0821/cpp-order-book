#ifndef _BOOK_L2_BASE_HPP
#define _BOOK_L2_BASE_HPP

#include "order.h"

class L2OrderBookBase {
   protected:
    void addOrderImpl(const Order* order);

    void cancelOrderImpl(const Order* order);

    void cancelOrderImpl(Side side, Price price, Quantity quantity);

    void printImpl();
};

#endif  // _BOOK_L2_BASE_HPP