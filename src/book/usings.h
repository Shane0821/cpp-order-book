#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <vector>

#include "util/objectpool.hpp"

using Price = double;
using Volume = double;
using Quantity = int64_t;
using OrderId = std::string;
using OrderIdView = std::string_view;
using Timestamp = std::chrono::time_point<std::chrono::high_resolution_clock>;