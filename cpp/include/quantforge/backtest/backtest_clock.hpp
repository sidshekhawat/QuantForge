#pragma once

#include "quantforge/market/timestamp.hpp"

namespace quantforge::backtest {

class BacktestClock {
public:
    explicit BacktestClock(market::Timestamp start_time) noexcept;

    [[nodiscard]] market::Timestamp now() const noexcept;

    void advance_to(market::Timestamp timestamp);

private:
    market::Timestamp current_time_;
};

} // namespace quantforge::backtest
