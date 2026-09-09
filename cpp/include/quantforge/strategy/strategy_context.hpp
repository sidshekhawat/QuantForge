#pragma once

#include "quantforge/market/timestamp.hpp"

namespace quantforge::strategy {

class StrategyContext {
public:
    explicit StrategyContext(market::Timestamp current_time) noexcept
        : current_time_(current_time) {}

    [[nodiscard]] market::Timestamp now() const noexcept {
        return current_time_;
    }

private:
    market::Timestamp current_time_;
};

} // namespace quantforge::strategy
