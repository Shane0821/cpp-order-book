#pragma once

#include "usings.h"

struct BinaryLevelSearcher {
    // find the first it where cmp returns false
    template <typename T, typename Compare>
    static decltype(auto) findLevelIt(T& levels, Price price, Compare cmp) {
        return std::lower_bound(
            levels.begin(), levels.end(), price,
            [cmp](const auto& level, auto price) { return cmp(level.price_, price); });
    }
};

struct BranchlessBinaryLevelSearcher {
    template <typename T, typename Compare>
    static decltype(auto) findLevelIt(T& levels, Price price, Compare cmp) {
        auto first = levels.begin();
        auto length = levels.end() - first;
        while (length > 0) {
            auto half = length / 2;
            first += cmp(first[half].price_, price) * (length - half);
            length = half;
        }
        return first;
    }
};

struct LinearLevelSearcher {
    // find the last it from right to left where cmp returns false
    template <typename T, typename Compare>
    static decltype(auto) findLevelIt(T& levels, Price price, Compare cmp) {
        auto it = levels.end();
        while (it != levels.begin()) {
            it--;
            if (cmp(it->price_, price)) {
                it++;
                break;
            }
        }
        return it;
    }
};