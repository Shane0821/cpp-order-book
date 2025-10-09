#pragma once

#include <limits>

#include "usings.h"

struct Constants {
    static constexpr Price InvalidPrice = std::numeric_limits<Price>::quiet_NaN();
};