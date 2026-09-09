#pragma once

#include "quantforge/backtest/backtest_clock.hpp"
#include "quantforge/market/bar.hpp"

#include <functional>
#include <vector>

namespace quantforge::backtest {

class BacktestEngine {
public:
    using BarHandler = std::function<void(const market::Bar&)>;

    explicit BacktestEngine(BacktestClock& clock) noexcept;

    void run(
        const std::vector<market::Bar>& bars,
        const BarHandler& handler
    );

private:
    BacktestClock& clock_;
};

} // namespace quantforge::backtest
