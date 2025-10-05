#ifndef _LEVEL_CONCEPTS_BASE_HPP
#define _LEVEL_CONCEPTS_BASE_HPP

#include <concepts>
#include <deque>
#include <list>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

// C++20 concept for LevelContainers

template <typename T>
concept LevelContainerBase = requires(T& c, const T& cc, typename T::iterator it) {
    typename T::iterator;
    typename T::const_iterator;
    typename T::value_type;
    typename T::reference;
    typename T::const_reference;
    { c.erase(it) } -> std::same_as<typename T::iterator>;
    { c.begin() } -> std::same_as<typename T::iterator>;
    { c.end() } -> std::same_as<typename T::iterator>;
    { cc.begin() } -> std::same_as<typename T::const_iterator>;
    { cc.end() } -> std::same_as<typename T::const_iterator>;
    { c.empty() } -> std::convertible_to<bool>;
};

static_assert(LevelContainerBase<std::vector<int>>);
static_assert(LevelContainerBase<std::list<int>>);
static_assert(LevelContainerBase<std::set<int>>);
static_assert(LevelContainerBase<std::unordered_set<int>>);
static_assert(LevelContainerBase<std::deque<int>>);

template <typename T>
concept ForwardLevelContainerBase =
    LevelContainerBase<T> && std::forward_iterator<typename T::iterator> &&
    std::forward_iterator<typename T::const_iterator>;

template <typename T>
concept BidirectLevelContainerBase =
    LevelContainerBase<T> && std::bidirectional_iterator<typename T::iterator> &&
    std::bidirectional_iterator<typename T::const_iterator>;

template <typename T>
concept AssociativeContainerBase =
    LevelContainerBase<T> && requires(T& c, const T& cc, typename T::iterator it) {
        typename T::node_type;
        { c.extract(it) } -> std::same_as<typename T::node_type>;
    };

template <typename T>
concept SingleAssociativeContainerBase =
    AssociativeContainerBase<T> &&
    requires(T& c, const T& cc, typename T::iterator it, typename T::value_type v,
             typename T::const_reference cr) {
        { c.insert(v) } -> std::same_as<std::pair<typename T::iterator, bool>>;
        { c.insert(cr) } -> std::same_as<std::pair<typename T::iterator, bool>>;
    };

template <typename T>
concept MultiAssociativeContainerBase =
    LevelContainerBase<T> &&
    requires(T& c, const T& cc, typename T::iterator it, typename T::value_type v,
             typename T::const_reference cr) {
        { c.insert(v) } -> std::same_as<typename T::iterator>;
        { c.insert(cr) } -> std::same_as<typename T::iterator>;
    };

#endif  // _LEVEL_CONCEPTS_BASE_HPP