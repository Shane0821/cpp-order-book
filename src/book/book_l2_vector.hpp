#ifndef _BOOK_L2_VECTOR_HPP
#define _BOOK_L2_VECTOR_HPP

#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "book_l2.hpp"
#include "level_searcher.hpp"

template <typename LevelSearcher = BinaryLevelSearcher, size_t MAX_DEPTH = 65536>
class VectorBasedL2OrderBook
    : public L2OrderBook<VectorBasedL2OrderBook<LevelSearcher, MAX_DEPTH>> {
    friend class L2OrderBook<VectorBasedL2OrderBook<LevelSearcher, MAX_DEPTH>>;

   public:
    VectorBasedL2OrderBook() {
        // reserve memory to avoid runtime reallocation
        bidLevels_.reserve(MAX_DEPTH);
        askLevels_.reserve(MAX_DEPTH);
    }

    ~VectorBasedL2OrderBook() = default;

   protected:
    void addOrderImpl(const Order* order) {
        if (order->side_ == Side::Buy) {
            levelAdd(bidLevels_, order->price_, order->remainingQuantity_,
                          std::less<Price>{});
        } else {
            levelAdd(askLevels_, order->price_, order->remainingQuantity_,
                          std::greater<Price>{});
        }
    }

    void cancelOrderImpl(const Order* order) {
        if (order->side_ == Side::Buy) {
            levelRemove(bidLevels_, order->price_, order->remainingQuantity_,
                             std::less<Price>{});
        } else {
            levelRemove(askLevels_, order->price_, order->remainingQuantity_,
                             std::greater<Price>{});
        }
    }

    void cancelOrderImpl(Side side, Price price, Quantity quantity) {
        if (side == Side::Buy) {
            levelRemove(bidLevels_, price, quantity, std::less<Price>{});
        } else {
            levelRemove(askLevels_, price, quantity, std::greater<Price>{});
        }
    }

    void printImpl() const {}

   private:
    template <typename T, typename Compare>
    void levelRemove(T& levels, Price price, Quantity quantity, Compare cmp) {
        auto it = LevelSearcher::findLevelIt(levels, price, cmp);

        if (it == levels.end() || it->price_ != price) [[unlikely]] {
            return;
        }

        auto& info = *it;
        info.quantity_ -= quantity;
        info.volume_ -= price * quantity;

        // erase the level if it's empty
        if (info.quantity_ <= 0) [[unlikely]] {
            levels.erase(it);
        }
    }

    template <typename T, typename Compare>
    void levelAdd(T& levels, Price price, Quantity quantity, Compare cmp) {
        auto it = LevelSearcher::findLevelIt(levels, price, cmp);

        if (it != levels.end() && it->price_ == price) [[likely]] {
            // level exists
            it->quantity_ += quantity;
            it->volume_ += price * quantity;
        } else {
            // level does not exist
            levels.insert(it, L2LevelInfo{price, quantity, price * quantity});
        }
    }

    using LevelInfos = std::vector<L2LevelInfo>;

    LevelInfos bidLevels_;  // largest price at the end
    LevelInfos askLevels_;  // smallest price at the end
};

#endif  // _BOOK_L2_VECTOR_HPP