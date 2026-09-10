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
    strategy::Strategy& strategy,
    signal::SignalSink& signal_sink)
{
    strategy::StrategyContext start_context(
        clock_.now(),
        signal_sink
    );

    strategy.on_start(start_context);

    for (const auto& bar : bars) {
        clock_.advance_to(bar.timestamp());

        strategy::StrategyContext bar_context(
            clock_.now(),
            signal_sink
        );

        strategy.on_bar(bar_context, bar);
    }

    strategy::StrategyContext finish_context(
        clock_.now(),
        signal_sink
    );

    strategy.on_finish(finish_context);
}

} // namespace quantforge::backtest
