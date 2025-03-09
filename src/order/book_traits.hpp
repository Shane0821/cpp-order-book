#include <set>
#include <unordered_set>

#include "order.hpp"

// Default LevelContainerTraits policy (e.g. vector, deque, list)
template <typename LevelContainer>
struct LevelContainerTraits {
    static LevelContainer::iterator insert(LevelContainer& container, const Order& order) {
        container.push_back(order);
        return container.end() - 1;
    }

    static void erase(LevelContainer& container, LevelContainer::iterator it) {
        container.erase(it);
    }
};

// Generic specialization for associative containers（e.g. std::set, std::unordered_set）
template <template <typename...> class Container, typename... Args>
struct LevelContainerTraits<Container<Order, Args...>> {
    using LevelContainer = Container<Order, Args...>;
    
    static LevelContainer::iterator insert(LevelContainer& container, const Order& order) {
        auto it = container.insert(order);
        return it;
    }

    static void erase(Container<Order, Args...>& container, typename LevelContainer::iterator it) {
        container.erase(it);
    }
};