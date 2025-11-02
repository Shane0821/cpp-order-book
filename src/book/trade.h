#pragma once

#include "usings.h"

struct TradeInfo {
    OrderId orderId_;
    Price price_;
    Quantity quantity_;
};

struct Trade {
    Trade(const TradeInfo &bidTrade, const TradeInfo &askTrade)
        : bidTrade_{bidTrade}, askTrade_{askTrade} {}

    TradeInfo bidTrade_;
    TradeInfo askTrade_;
};

using Trades = std::vector<Trade>;