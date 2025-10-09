#ifndef _BOOK_L3_HPP
#define _BOOK_L3_HPP

#include "book_l3_base.hpp"
#include "level_info.h"
#include "level_traits.hpp"
#include "order.h"
#include "order_modify.h"
#include "trade.h"
#include "util/objectpool.hpp"

template <typename Derived, typename Base = L3OrderBookBase>
class L3OrderBook : public L3OrderBookBase {
   public:
    using OrderCancelCallback = std::function<void(const Order *)>;
    using OrderAddCallback = std::function<void(const Order *)>;

    L3OrderBook() = default;
    virtual ~L3OrderBook() = default;

    // add order to the order book
    Trades addOrder(Order *order) {
        // type check
        static_assert(std::is_same_v<decltype(order->price_), Price>);
        static_assert(std::is_same_v<decltype(order->initialQuantity_), Quantity>);
        static_assert(std::is_same_v<decltype(order->remainingQuantity_), Quantity>);
        static_assert(std::is_same_v<decltype(order->getOrderId()), OrderId>);

        if (order == nullptr || order->remainingQuantity_ == 0 || order->price_ < 0 ||
            static_cast<const Derived *>(this)->orderExistsImpl(order->getOrderId()))
            [[unlikely]] {
            return {};
        }

        if (order->getOrderType() == OrderType::Market) {
            // the worst price is only used for matching
            // the actual price executed is on the other side
            if (order->getSide() == Side::Buy && !isAskEmpty()) {
                const auto &[worstAskPrice, _] =
                    static_cast<Derived *>(this)->getWorstAskLevelImpl();
                order->toGoodTillCancel(worstAskPrice);
            } else if (order->getSide() == Side::Sell && !isBidEmpty()) {
                const auto &[worstBidPrice, _] =
                    static_cast<Derived *>(this)->getWorstBidLevelImpl();
                order->toGoodTillCancel(worstBidPrice);
            } else {
                return {};
            }
        }

        if (order->getOrderType() == OrderType::FillAndKill &&
            !canMatch(order->getSide(), order->getPrice())) {
            return {};
        }

        if (order->getOrderType() == OrderType::FillOrKill &&
            !canFullyFill(order->getSide(), order->getPrice(),
                          order->getInitialQuantity())) {
            return {};
        }

        static_cast<Derived *>(this)->addOrderImpl(order);
        onOrderAdded(order);
        return MatchOrders();
    }

    // cancel order by order ID
    void cancelOrder(const OrderId &orderId) {
        if (!static_cast<Derived *>(this)->orderExistsImpl(orderId)) [[unlikely]] {
            return;
        }
        Order *order = static_cast<Derived *>(this)->cancelOrderImpl(orderId);
        onOrderCancelled(order);
    }

    template <typename OrderIds>
        requires std::same_as<OrderIds::value_type, std::string>
    void cancelOrders(OrderIds &&orderIds) {
        for (const auto &orderId : orderIds) {
            cancelOrder(orderId);
        }
    }

    Trades modifyOrder(const OrderId &orderId, const OrderModify &modify) {
        if (!static_cast<Derived *>(this)->orderExistsImpl(orderId)) [[unlikely]] {
            return {};
        }
        Order *order = static_cast<Derived *>(this)->cancelOrderImpl(orderId);
        modify.toOrderPointer(order);
        return static_cast<Derived *>(this)->addOrderImpl(order);
    }

    size_t getOrderCount() const {
        return static_cast<const Derived *>(this)->getOrderCountImpl();
    }

    void print() const { static_cast<const Derived *>(this)->printImpl(); }

   protected:
    // print the order book
    bool isBidEmpty() const {
        return static_cast<const Derived *>(this)->bidLevels_.empty();
    }
    bool isAskEmpty() const {
        return static_cast<const Derived *>(this)->askLevels_.empty();
    }

    // void PruneGoodForDayOrders();

    bool canFullyFill(Side side, Price price, Quantity quantity) const {
        if (!canMatch(side, price)) return false;

        Price threshold;

        if (side == Side::Buy) {
            const auto &[askPrice, _] =
                static_cast<Derived *>(this)->getBestAskLevelImpl();
            threshold = askPrice;
        } else {
            const auto [bidPrice, _] =
                static_cast<Derived *>(this)->getBestBidLevelImpl();
            threshold = bidPrice;
        }

        for (const auto &[levelPrice, l2Info] : data_) {
            if ((side == Side::Buy && levelPrice > price) ||
                (side == Side::Sell && levelPrice < price))
                break;

            if (quantity <= l2Info.quantity_) return true;

            quantity -= l2Info.quantity_;
        }

        return false;
    }

    bool canMatch(Side side, Price price) const {
        if (side == Side::Buy) {
            if (isAskEmpty()) [[unlikely]] {
                return false;
            }
            const auto &[askPrice, _] =
                static_cast<Derived *>(this)->getBestAskLevelImpl();
            return askPrice <= price;
        } else {
            if (isBidEmpty()) [[unlikely]] {
                return false;
            }
            const auto &[bidPrice, _] =
                static_cast<Derived *>(this)->getBestBidLevelImpl();
            return bidPrice >= price;
        }
    }

    Trades MatchOrders() {
        Trades trades;

        while (true) {
            if (isBidEmpty() || isAskEmpty()) break;

            auto &[bidPrice, bids] = static_cast<Derived *>(this)->getBestBidLevelImpl();
            auto &[askPrice, asks] = static_cast<Derived *>(this)->getBestAskLevelImpl();

            // no cross
            if (bidPrice < askPrice) break;

            using BidLevelContainer = std::remove_reference_t<decltype(bids)>;
            using AskLevelContainer = std::remove_reference_t<decltype(asks)>;

            while (!bids.empty() && !asks.empty()) {
                auto bidIt = LevelContainerTraits<BidLevelContainer>::first(bids);
                auto askIt = LevelContainerTraits<AskLevelContainer>::first(asks);

                auto bid = *bidIt;
                auto ask = *askIt;

                Quantity quantity =
                    std::min(bid->getRemainingQuantity(), ask->getRemainingQuantity());

                bid->fill(quantity);
                ask->fill(quantity);

                trades.emplace_back(TradeInfo{bid->getOrderId(), bidPrice, quantity},
                                    TradeInfo{ask->getOrderId(), askPrice, quantity});

                // update callback
                onOrderMatched(bidPrice, quantity, bid->isFilled());
                onOrderMatched(askPrice, quantity, ask->isFilled());

                if (bid->isFilled()) {
                    static_cast<Derived *>(this)->levelCancelOrderImpl(bids, bidIt);
                    // TODO: recycle bid
                }

                if (ask->isFilled()) {
                    static_cast<Derived *>(this)->levelCancelOrderImpl(asks, askIt);
                    // TODO: recycle ask
                }
            }

            if (LevelContainerTraits<BidLevelContainer>::empty(bids)) {
                static_cast<Derived *>(this)->removeEmptyBidLevelImpl(bidPrice);
            }

            if (LevelContainerTraits<AskLevelContainer>::empty(asks)) {
                static_cast<Derived *>(this)->removeEmptyAskLevelImpl(askPrice);
            }
        }

        if (!isBidEmpty()) {
            Order *order = static_cast<Derived *>(this)->getBestBidImpl();
            if (order->getOrderType() == OrderType::FillAndKill) {
                cancelOrder(order->getOrderId());
            }
        }

        if (!isAskEmpty()) {
            Order *order = static_cast<Derived *>(this)->getBestAskImpl();
            if (order->getOrderType() == OrderType::FillAndKill) {
                cancelOrder(order->getOrderId());
            }
        }
        return trades;
    }

    void onOrderCancelled(const Order *order) {}
    void onOrderAdded(const Order *order) {}
    void onOrderMatched(Price price, Quantity quantity, bool isFullyFilled) {}

    OrderCancelCallback orderCancelCallback_{nullptr};
    OrderAddCallback orderAddCallback_{nullptr};
};

#endif  // _BOOK_L3_HPP