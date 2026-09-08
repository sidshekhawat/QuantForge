#include "quantforge/backtest/backtest_clock.hpp"

#include <stdexcept>

namespace quantforge::backtest {

BacktestClock::BacktestClock(market::Timestamp start_time) noexcept
    : current_time_(start_time) {}

market::Timestamp BacktestClock::now() const noexcept {
    return current_time_;
}

void BacktestClock::advance_to(market::Timestamp timestamp) {
    if (timestamp < current_time_) {
        throw std::logic_error(
            "Backtest clock cannot move backwards."
        );
    }

    current_time_ = timestamp;
}

} // namespace quantforge::backtest
