#pragma once

#include "usings.h"

struct L2LevelInfo {
    Price price_;
    Quantity quantity_;
    Volume volume_;
};

template <typename LevelContainer>
struct L3LevelInfo {
    Price price_;
    LevelContainer levelContainer_;
};