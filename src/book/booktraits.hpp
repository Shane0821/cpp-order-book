#ifndef _BOOK_TRAITS_HPP
#define _BOOK_TRAITS_HPP

#include "bookconcepts.hpp"
#include "order.hpp"

template <typename LevelContainer>
    requires LevelContainerBaseConcept<LevelContainer>
struct LevelContainerTraits {
    static LevelContainer::iterator insert(LevelContainer& container, const Order& order)
        requires BiDirectionLevelContainerConcept<LevelContainer>

    {
        container.push_back(order);
        return std::prev(container.end());
    }

    static LevelContainer::iterator insert(LevelContainer& container, const Order& order)
        requires SetBasedContainerConcept<LevelContainer>
    {
        auto [it, success] = container.insert(order);
        return it;
    }

    static LevelContainer::iterator insert(LevelContainer& container, const Order& order)
        requires MultiSetBasedContainerConcept<LevelContainer>
    {
        auto it = container.insert(order);
        return it;
    }

    static void erase(LevelContainer& container, const LevelContainer::iterator& it) {
        container.erase(it);
    }

    static bool empty(LevelContainer& container) { return container.empty(); }

    static LevelContainer::const_iterator first(const LevelContainer& container) {
        return container.cbegin();
    }
};

// assertions
template <typename T>
constexpr bool has_valid_insert_v = requires(T container, const Order& order) {
    {
        LevelContainerTraits<T>::insert(container, order)
    } -> std::same_as<typename T::iterator>;
};

static_assert(!has_valid_insert_v<std::vector<Order>>,
              "vector doesn't have valid insert");
static_assert(has_valid_insert_v<std::deque<Order>>, "deque has valid insert");
static_assert(has_valid_insert_v<std::list<Order>>, "list has valid insert");
static_assert(has_valid_insert_v<std::set<Order>>, "set has valid insert");
static_assert(has_valid_insert_v<std::unordered_set<Order, Order::Hash, Order::Equal>>,
              "unordered_set has valid insert");

#endif  // _BOOK_TRAITS_HPP