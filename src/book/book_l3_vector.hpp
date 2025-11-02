#ifndef _BOOK_L3_VECTOR_HPP
#define _BOOK_L3_VECTOR_HPP

#include "book_l2_map.hpp"
#include "book_l2_vector.hpp"
#include "book_l3.hpp"
#include "level_searcher.hpp"

// NOTE: do not use vector for level container (because of it invalidation)
template <LevelContainerBase LevelContainer, typename LevelSearcher = BinaryLevelSearcher,
          typename L2BookInternal = MapBasedL2OrderBook, size_t MAX_DEPTH = 65536>
    requires std::same_as<typename LevelContainer::value_type, Order*> &&
             std::default_initializable<L2BookInternal>
class VectorBasedL3OrderBook
    : public L3OrderBook<VectorBasedL3OrderBook<LevelContainer, LevelSearcher,
                                                L2BookInternal, MAX_DEPTH>> {
    friend class L3OrderBook<
        VectorBasedL3OrderBook<LevelContainer, LevelSearcher, L2BookInternal, MAX_DEPTH>>;

   public:
    VectorBasedL3OrderBook() {
        // reserve memory to avoid runtime reallocation
        bidLevels_.reserve(MAX_DEPTH);
        askLevels_.reserve(MAX_DEPTH);
    }

    ~VectorBasedL3OrderBook() = default;

   protected:
    void addOrderImpl(Order* order) {
        if (order->side_ == Side::Buy) {
            oidToLevelContainerItMap_[order->orderId_] =
                levelAddOrder(bidLevels_, order, std::less<Price>{});
        } else {
            oidToLevelContainerItMap_[order->orderId_] =
                levelAddOrder(askLevels_, order, std::greater<Price>{});
        }
    }

    Order* cancelOrderImpl(const OrderId& orderId) {
        auto levelContainerIt = oidToLevelContainerItMap_[orderId];
        Order* order = *levelContainerIt;

        if (order->side_ == Side::Buy) {
            levelRemoveOrder(bidLevels_, levelContainerIt, order->price_,
                             std::less<Price>{});
        } else {
            levelRemoveOrder(askLevels_, levelContainerIt, order->price_,
                             std::greater<Price>{});
        }

        // erase order ID from the hashmap
        oidToLevelContainerItMap_.erase(orderId);
        return order;
    }

    // only does cancelling order from level
    // does not remove empty level
    void levelRemoveOrderImpl(LevelContainer& levelContainer,
                              typename LevelContainer::iterator levelContainerIt) {
        LevelContainerTraits<LevelContainer>::erase(levelContainer, levelContainerIt);
        oidToLevelContainerItMap_.erase((*levelContainerIt)->orderId_);
    }

    void removeEmptyBidLevelImpl(Price price) {
        auto it = LevelSearcher::findLevelIt(bidLevels_, price, std::less<Price>{});
        bidLevels_.erase(it);
    }
    void removeEmptyAskLevelImpl(Price price) {
        auto it = LevelSearcher::findLevelIt(askLevels_, price, std::greater<Price>{});
        askLevels_.erase(it);
    }

    Order* getBestBidImpl() {
        return *LevelContainerTraits<LevelContainer>::first(
            bidLevels_.rbegin()->levelContainer_);
    }
    Order* getBestAskImpl() {
        return *LevelContainerTraits<LevelContainer>::first(
            askLevels_.rbegin()->levelContainer_);
    }

    decltype(auto) getBestBidLevelImpl() { return *bidLevels_.rbegin(); }
    decltype(auto) getBestAskLevelImpl() { return *askLevels_.rbegin(); }

    decltype(auto) getWorstBidLevelImpl() { return *bidLevels_.begin(); };
    decltype(auto) getWorstAskLevelImpl() { return *askLevels_.begin(); };

    const L2BookInternal* getL2BookImpl() const noexcept { return &l2_book_; }
    L2BookInternal* getL2BookImpl() noexcept { return &l2_book_; }

    bool orderExistsImpl(const OrderId& orderId) const {
        return oidToLevelContainerItMap_.contains(orderId);
    }

    void printImpl() const {}

   private:
    template <typename T, typename Compare>
    decltype(auto) levelAddOrder(T& levels, Order* order, Compare cmp) {
        auto it = LevelSearcher::findLevelIt(levels, order->price_, cmp);

        if (it != levels.end() && it->price_ == order->price_) [[likely]] {
            // price level exists, insert order into the level
            return LevelContainerTraits<LevelContainer>::insert(it->levelContainer_,
                                                                order);
        }

        // price level doesn't exist, create a new level
        auto vecIt = levels.insert(it, L3LevelInfo{order->price_, LevelContainer{}});
        return LevelContainerTraits<LevelContainer>::insert(vecIt->levelContainer_,
                                                            order);
    }

    template <typename T, typename Iterator, typename Compare>
    void levelRemoveOrder(T& levels, Iterator levelContainerIt, Price price,
                          Compare cmp) {
        auto vecIt = LevelSearcher::findLevelIt(levels, price, cmp);
        auto& levelContainer = vecIt->levelContainer_;

        // erase order from the level
        LevelContainerTraits<LevelContainer>::erase(levelContainer, levelContainerIt);

        // erase the level if it's empty
        if (LevelContainerTraits<LevelContainer>::empty(levelContainer)) [[unlikely]] {
            // erase the level from the vector
            levels.erase(vecIt);
        }
    }

    using Levels = std::vector<L3LevelInfo<LevelContainer>>;
    Levels bidLevels_;  // largest price at the end
    Levels askLevels_;  // smallest price at the end

    std::unordered_map<OrderId, typename LevelContainer::iterator>
        oidToLevelContainerItMap_;

    L2BookInternal l2_book_;
};

#endif  // _BOOK_L3_VECTOR_HPP