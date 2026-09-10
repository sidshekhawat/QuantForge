#pragma once

#include "quantforge/market/timestamp.hpp"
#include "quantforge/signal/signal.hpp"
#include "quantforge/signal/signal_sink.hpp"

namespace quantforge::strategy {

class StrategyContext {
public:
    StrategyContext(
        market::Timestamp current_time,
        signal::SignalSink& signal_sink
    ) noexcept
        : current_time_(current_time),
          signal_sink_(signal_sink) {}

    [[nodiscard]] market::Timestamp now() const noexcept {
        return current_time_;
    }

    void emit_signal(const signal::Signal& signal) const {
        signal_sink_.emit(signal);
    }

private:
    market::Timestamp current_time_;
    signal::SignalSink& signal_sink_;
};

} // namespace quantforge::strategy
