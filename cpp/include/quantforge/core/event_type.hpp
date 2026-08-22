#pragma once

namespace quantforge::core {

enum class EventType {
    MarketData,
    Signal,
    Order,
    Fill,
    PortfolioUpdate,
    System
};

}  // namespace quantforge::core