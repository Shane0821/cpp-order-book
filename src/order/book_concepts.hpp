#include <concepts>

#include "order.hpp"

// C++20 concept for LevelContainer. Must have insert method.
template <typename T>
concept LevelContainerConcept =
    requires(T container, const Order &order, typename T::iterator it) {
        { container.insert(order) } -> std::same_as<typename T::iterator>;
        { container.erase(it) } -> std::same_as<void>;
    };