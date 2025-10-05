#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "util/sso_string.h"

using Price = double;
using Volume = double;
using Quantity = int64_t;
using OrderId = String;
using Timestamp = std::chrono::time_point<std::chrono::high_resolution_clock>;