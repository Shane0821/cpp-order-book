#ifndef _BOOK_L2_BASE_HPP
#define _BOOK_L2_BASE_HPP

#include "order.h"

class L2OrderBookBase {
   protected:
    void addOrderImpl(const Order* order);

    void cancelOrderImpl(const Order* order);

    void cancelOrderImpl(Side side, Price price, Quantity quantity);

    decltype(auto) bidBegin();
    decltype(auto) askBegin();

    decltype(auto) bidEnd();
    decltype(auto) askEnd();

    void printImpl();
};

#endif  // _BOOK_L2_BASE_HPP