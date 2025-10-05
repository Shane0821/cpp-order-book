#ifndef _LEVEL_CONCEPTS_HPP
#define _LEVEL_CONCEPTS_HPP

#include "level_concepts_base.hpp"

// C++20 concept for LevelContainers

// vector, ...
template <typename T>
concept ForwardSequencialLevelContainer =
    ForwardLevelContainerBase<T> && requires(T& c, const T& cc, typename T::value_type v,
                                             typename T::const_reference cr) {
        { c.push_back(v) } -> std::same_as<void>;
        { c.push_back(cr) } -> std::same_as<void>;
        { c.pop_back() } -> std::same_as<void>;
        { c.back() } -> std::same_as<typename T::reference>;
        { cc.back() } -> std::same_as<typename T::const_reference>;
    };

static_assert(ForwardSequencialLevelContainer<std::vector<int>>);
static_assert(ForwardSequencialLevelContainer<std::list<int>>);
static_assert(!ForwardSequencialLevelContainer<std::set<int>>);
static_assert(!ForwardSequencialLevelContainer<std::unordered_set<int>>);
static_assert(ForwardSequencialLevelContainer<std::deque<int>>);

// deque, list
template <typename T>
concept BidirectSequencialLevelContainer =
    BidirectLevelContainerBase<T> && ForwardSequencialLevelContainer<T> &&
    requires(T& c, const T& cc, typename T::value_type v,
             typename T::const_reference cr) {
        { c.push_front(v) } -> std::same_as<void>;
        { c.push_front(cr) } -> std::same_as<void>;
        { c.pop_front() } -> std::same_as<void>;
        { c.front() } -> std::same_as<typename T::reference>;
        { cc.front() } -> std::same_as<typename T::const_reference>;
    };

static_assert(!BidirectSequencialLevelContainer<std::vector<int>>);
static_assert(BidirectSequencialLevelContainer<std::list<int>>);
static_assert(!BidirectSequencialLevelContainer<std::set<int>>);
static_assert(!BidirectSequencialLevelContainer<std::unordered_set<int>>);
static_assert(BidirectSequencialLevelContainer<std::deque<int>>);

// unordered set
template <typename T>
concept HashmapBasedAssociativeContainer =
    ForwardLevelContainerBase<T> && SingleAssociativeContainerBase<T> && requires(T& c) {
        typename T::hasher;
        typename T::key_equal;
    };

static_assert(!HashmapBasedAssociativeContainer<std::vector<int>>);
static_assert(!HashmapBasedAssociativeContainer<std::list<int>>);
static_assert(!HashmapBasedAssociativeContainer<std::set<int>>);
static_assert(HashmapBasedAssociativeContainer<std::unordered_set<int>>);
static_assert(!HashmapBasedAssociativeContainer<std::deque<int>>);

template <typename T>
concept SetBasedAssociativeContainer =
    BidirectLevelContainerBase<T> && SingleAssociativeContainerBase<T>;

static_assert(!SetBasedAssociativeContainer<std::vector<int>>);
static_assert(!SetBasedAssociativeContainer<std::list<int>>);
static_assert(SetBasedAssociativeContainer<std::set<int>>);
static_assert(!SetBasedAssociativeContainer<std::unordered_set<int>>);
static_assert(!SetBasedAssociativeContainer<std::deque<int>>);

// unordered multi set
template <typename T>
concept MultiHashmapBasedAssociativeContainer =
    ForwardLevelContainerBase<T> && MultiAssociativeContainerBase<T> && requires(T& c) {
        typename T::hasher;
        typename T::key_equal;
    };

static_assert(!MultiHashmapBasedAssociativeContainer<std::vector<int>>);
static_assert(!MultiHashmapBasedAssociativeContainer<std::list<int>>);
static_assert(!MultiHashmapBasedAssociativeContainer<std::set<int>>);
static_assert(!MultiHashmapBasedAssociativeContainer<std::unordered_set<int>>);
static_assert(!MultiHashmapBasedAssociativeContainer<std::multiset<int>>);
static_assert(MultiHashmapBasedAssociativeContainer<std::unordered_multiset<int>>);
static_assert(!MultiHashmapBasedAssociativeContainer<std::deque<int>>);

// multiset
template <typename T>
concept MultiSetBasedAssociativeContainer =
    BidirectLevelContainerBase<T> && MultiAssociativeContainerBase<T>;

static_assert(!MultiSetBasedAssociativeContainer<std::vector<int>>);
static_assert(!MultiSetBasedAssociativeContainer<std::list<int>>);
static_assert(!MultiSetBasedAssociativeContainer<std::set<int>>);
static_assert(!MultiSetBasedAssociativeContainer<std::unordered_set<int>>);
static_assert(MultiSetBasedAssociativeContainer<std::multiset<int>>);
static_assert(!MultiSetBasedAssociativeContainer<std::unordered_multiset<int>>);
static_assert(!MultiSetBasedAssociativeContainer<std::deque<int>>);

#endif  // _LEVEL_CONCEPTS_HPP