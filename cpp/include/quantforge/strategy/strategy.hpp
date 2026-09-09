#pragma once

#include "quantforge/market/bar.hpp"
#include "quantforge/strategy/strategy_context.hpp"

namespace quantforge::strategy {

class Strategy {
public:
    virtual ~Strategy() = default;

    virtual void on_start(const StrategyContext& context) {}

    virtual void on_bar(
        const StrategyContext& context,
        const market::Bar& bar
    ) = 0;

    virtual void on_finish(const StrategyContext& context) {}
};

} // namespace quantforge::strategy
