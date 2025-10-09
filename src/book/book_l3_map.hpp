#ifndef _BOOK_L3_MAP_HPP
#define _BOOK_L3_MAP_HPP

#include "book_l3.hpp"
#include "book_l2_map.hpp"
#include "book_l2_vector.hpp"
#include "level_searcher.hpp"

// NOTE: do not use vector for level container (because of it invalidation)
template <typename LevelContainer, typename L2Book>
    requires std::same_as<typename LevelContainer::value_type, Order*>
class MapBasedL3OrderBook : public L3OrderBook<MapBasedL3OrderBook<LevelContainer>> {
    friend class L3OrderBook<MapBasedL3OrderBook<LevelContainer>>;

   public:
    MapBasedL3OrderBook() = default;
    ~MapBasedL3OrderBook() = default;

   protected:
    void addOrderImpl(Order* order) {
        if (order->side_ == Side::Buy) {
            oidToLevelContainerItMap_[order->orderId_] =
                addOrderToMap(bidLevels_, priceToBidLevelItMap_, order);
        } else {
            oidToLevelContainerItMap_[order->orderId_] =
                addOrderToMap(askLevels_, priceToAskLevelItMap_, order);
        }
    }

    Order* cancelOrderImpl(const OrderId& orderId) {
        auto levelContainerIt = oidToLevelContainerItMap_[orderId];
        Order* order = *levelContainerIt;

        if (order->side_ == Side::Buy) {
            cancelOrderFromMap(bidLevels_, priceToBidLevelItMap_, levelContainerIt,
                               order->price_);
        } else {
            cancelOrderFromMap(askLevels_, priceToAskLevelItMap_, levelContainerIt,
                               order->price_);
        }
        // erase order ID
        oidToLevelContainerItMap_.erase(orderId);

        return order;
    }

    // only does cancelling order from level
    // does not remove empty level
    void levelCancelOrderImpl(LevelContainer& levelContainer,
                              typename LevelContainer::iterator levelContainerIt) {
        LevelContainerTraits<LevelContainer>::erase(levelContainer, levelContainerIt);
        oidToLevelContainerItMap_.erase((*levelContainerIt)->orderId_);
    }

    void removeEmptyBidLevelImpl(Price price) {
        bidLevels_.erase(priceToBidLevelItMap_[price]);
        priceToBidLevelItMap_.erase(price);
    }
    void removeEmptyAskLevelImpl(Price price) {
        askLevels_.erase(priceToAskLevelItMap_[price]);
        priceToAskLevelItMap_.erase(price);
    }

    Order* getBestBidImpl() {
        return *LevelContainerTraits<LevelContainer>::first(bidLevels_.begin()->second);
    }
    Order* getBestAskImpl() {
        return *LevelContainerTraits<LevelContainer>::first(askLevels_.begin()->second);
    }

    decltype(auto) getBestBidLevelImpl() { return *bidLevels_.begin(); }
    decltype(auto) getBestAskLevelImpl() { return *askLevels_.begin(); }

    decltype(auto) getWorstBidLevelImpl() { return *bidLevels_.begin(); };
    decltype(auto) getWorstAskLevelImpl() { return *askLevels_.begin(); };

    bool orderExistsImpl(const OrderId& orderId) const {
        return oidToLevelContainerItMap_.contains(orderId);
    }

    auto getOrderCountImpl() const { return oidToLevelContainerItMap_.size(); }

    void printImpl() const {}

    template <typename T, typename M>
    decltype(auto) addOrderToMap(T& levels, M& price2levelIt, Order* order) {
        if (price2levelIt.contains(order->price_)) [[likely]] {
            auto& levelContainer = price2levelIt[order->price_]->second;
            return LevelContainerTraits<LevelContainer>::insert(levelContainer, order);
        }
        // create a new level
        auto [it, inserted] = levels.emplace(order->price_, LevelContainer{});
        price2levelIt[order->price_] = it;

        // insert order into the level
        // it->second is the container
        return LevelContainerTraits<LevelContainer>::insert(it->second, order);
    }

    template <typename T, typename M, typename Iterator>
    void cancelOrderFromMap(T& levels, M& price2levelIt, Iterator levelContainerIt,
                            Price price) {
        auto levelIt = price2levelIt[price];
        auto& levelContainer = levelIt->second;
        // erase order from the level
        LevelContainerTraits<LevelContainer>::erase(levelContainer, levelContainerIt);

        // erase the level if it's empty
        if (LevelContainerTraits<LevelContainer>::empty(levelContainer)) [[unlikely]] {
            levels.erase(levelIt);
            price2levelIt.erase(price);
        }
    }

    std::map<Price, LevelContainer, std::greater<Price>> bidLevels_;
    std::map<Price, LevelContainer, std::less<Price>> askLevels_;

    std::unordered_map<Price, typename decltype(bidLevels_)::iterator>
        priceToBidLevelItMap_;
    std::unordered_map<Price, typename decltype(askLevels_)::iterator>
        priceToAskLevelItMap_;
    std::unordered_map<OrderId, typename LevelContainer::iterator>
        oidToLevelContainerItMap_;
};

#endif  // _BOOK_L3_MAP_HPP