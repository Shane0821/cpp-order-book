#ifndef _BOOK_TRAITS_HPP
#define _BOOK_TRAITS_HPP

#include "book_concepts.hpp"
#include "order.hpp"

// Primary template
template <typename LevelContainer>
struct LevelContainerTraits {};

// deque, list, etc.
template <typename LevelContainer>
    requires std::same_as<LevelContainer, std::deque<Order>> ||
             std::same_as<LevelContainer, std::list<Order>> ||
             DoubleDirectionLevelContainerConcept<LevelContainer>
struct LevelContainerTraits {
    static LevelContainer::iterator insert(LevelContainer& container,
                                           const Order& order) {
        container.push_back(order);
        return std::prev(container.end());
    }

    static void erase(LevelContainer& container, LevelContainer::iterator it) {
        container.erase(it);
    }

    static LevelContainer::iterator first(LevelContainer& container) {
        return container.begin();
    }
};

// vector, etc.
template <typename LevelContainer>
    requires std::same_as<LevelContainer, std::vector<Order>> ||
             VectorBasedLevelContainerConcept<LevelContainer>
struct LevelContainerTraits {
    static LevelContainer::iterator insert(LevelContainer& container,
                                           const Order& order) {
        container.push_back(order);
        return std::prev(container.end());
    }

    static void erase(LevelContainer& container, LevelContainer::iterator it) {
        container.erase(it);
    }

    static LevelContainer::iterator first(LevelContainer& container) {
        return std::prev(container.end());
    }
};

// std::set, std::unordered_set, etc.
template <typename LevelContainer>
    requires LevelContainerBaseConcept<LevelContainer>
struct LevelContainerTraits {
    static LevelContainer::iterator insert(LevelContainer& container,
                                           const Order& order) {
        auto it = container.insert(order);
        return it;
    }

    static void erase(LevelContainer& container, typename LevelContainer::iterator it) {
        container.erase(it);
    }

    static LevelContainer::iterator first(LevelContainer& container) {
        return container.begin();
    }
};

#endif  // _BOOK_TRAITS_HPP