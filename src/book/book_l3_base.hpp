#ifndef _BOOK_L3_BASE_HPP
#define _BOOK_L3_BASE_HPP

#include "order.h"

class L3OrderBookBase {
   protected:
    void addOrderImpl(Order *order);

    Order *cancelOrderImpl(const OrderId &orderId);

    template <typename LevelContainer>
    void levelCancelOrderImpl(LevelContainer &levelContainer,
                              typename LevelContainer::iterator levelContainerIt);

    void removeEmptyBidLevelImpl(Price price);
    void removeEmptyAskLevelImpl(Price price);

    Order *getBestBidImpl();
    Order *getBestAskImpl();

    decltype(auto) getBestBidLevelImpl();
    decltype(auto) getBestAskLevelImpl();

    decltype(auto) getWorstBidLevelImpl();
    decltype(auto) getWorstAskLevelImpl();

    bool orderExistsImpl(const OrderId &orderId) const;

    size_t getOrderCountImpl() const;

    void printImpl() const;
};

#endif  // _BOOK_L3_BASE_HPP