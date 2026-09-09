#include "quantforge/backtest/backtest_engine.hpp"

#include <stdexcept>

namespace quantforge::backtest {

BacktestEngine::BacktestEngine(BacktestClock& clock) noexcept
    : clock_(clock) {}

void BacktestEngine::run(
    const std::vector<market::Bar>& bars,
    const BarHandler& handler)
{
    if (!handler) {
        throw std::invalid_argument(
            "Backtest engine requires a bar handler."
        );
    }

    for (const auto& bar : bars) {
        clock_.advance_to(bar.timestamp());
        handler(bar);
    }
}

void BacktestEngine::run(
    const std::vector<market::Bar>& bars,
    strategy::Strategy& strategy)
{
    strategy.on_start();

    for (const auto& bar : bars) {
        clock_.advance_to(bar.timestamp());
        strategy.on_bar(bar);
    }

    strategy.on_finish();
}

} // namespace quantforge::backtest
