#ifndef _LEVEL_TRAITS_HPP
#define _LEVEL_TRAITS_HPP

#include "level_concepts.hpp"

template <LevelContainerBase LevelContainer>
struct LevelContainerTraits {
    static LevelContainer::iterator insert(LevelContainer& container,
                                           typename LevelContainer::value_type order)
        requires ForwardSequencialLevelContainer<LevelContainer>

    {
        container.push_back(order);
        return std::prev(container.end());
    }

    static LevelContainer::iterator insert(LevelContainer& container,
                                           typename LevelContainer::value_type order)
        requires SetBasedAssociativeContainer<LevelContainer>
    {
        auto [it, success] = container.insert(order);
        return it;
    }

    static LevelContainer::iterator insert(LevelContainer& container,
                                           typename LevelContainer::value_type order)
        requires MultiSetBasedAssociativeContainer<LevelContainer>
    {
        auto it = container.insert(order);
        return it;
    }

    static void erase(LevelContainer& container, LevelContainer::iterator it) {
        container.erase(it);
    }

    static bool empty(const LevelContainer& container) { return container.empty(); }

    static LevelContainer::const_iterator first(const LevelContainer& container) {
        return container.cbegin();
    }

    static LevelContainer::iterator first(LevelContainer& container) {
        return container.begin();
    }
};

// assertions
template <typename T>
constexpr bool has_valid_insert_v = requires(T container, typename T::value_type order) {
    {
        LevelContainerTraits<T>::insert(container, order)
    } -> std::same_as<typename T::iterator>;
};

#endif  // _LEVEL_TRAITS_HPP