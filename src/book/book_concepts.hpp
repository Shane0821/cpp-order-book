#ifndef _BOOK_CONCEPTS_HPP
#define _BOOK_CONCEPTS_HPP

#include <concepts>
#include <deque>
#include <list>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

#include "order.hpp"

// C++20 concept for LevelContainers

template <typename T>
concept LevelContainerBaseConcept = requires(T container, typename T::iterator it) {
    { container.erase(it) } -> std::same_as<typename T::iterator>;
    { container.begin() } -> std::same_as<typename T::iterator>;
    { container.end() } -> std::same_as<typename T::iterator>;
};

static_assert(LevelContainerBaseConcept<std::set<Order>>);
static_assert(LevelContainerBaseConcept<std::unordered_set<Order>>);

template <typename T>
concept DoubleDirectionLevelContainerConcept =
    LevelContainerBaseConcept<T> &&
    requires(T container, const Order &order, typename T::iterator it) {
        { container.push_back(order) } -> std::same_as<void>;
        { container.push_front(order) } -> std::same_as<void>;
        { container.pop_back() } -> std::same_as<void>;
        { container.pop_front() } -> std::same_as<void>;
        { container.back() } -> std::same_as<Order &>;
        { container.front() } -> std::same_as<Order &>;
    };

static_assert(DoubleDirectionLevelContainerConcept<std::deque<Order>>);
static_assert(DoubleDirectionLevelContainerConcept<std::list<Order>>);

template <typename T>
concept VectorBasedLevelContainerConcept =
    LevelContainerBaseConcept<T> &&
    requires(T container, const Order &order, typename T::iterator it) {
        { container.push_back(order) } -> std::same_as<void>;
        { container.pop_back() } -> std::same_as<void>;
        { container.back() } -> std::same_as<Order &>;
    } && !requires(T container, const Order &order) {
        container.push_front(order);
        container.pop_front();
        container.front();
    };

static_assert(VectorBasedLevelContainerConcept<std::vector<Order>>);

#endif  // _BOOK_CONCEPTS_HPP