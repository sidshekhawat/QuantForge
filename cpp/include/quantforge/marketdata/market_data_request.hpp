#pragma once

#include "quantforge/market/instrument_id.hpp"
#include "quantforge/market/timeframe.hpp"
#include "quantforge/market/timestamp.hpp"

namespace quantforge::marketdata {

class MarketDataRequest {
public:
    constexpr MarketDataRequest(
        market::InstrumentId instrument_id,
        market::Timestamp start,
        market::Timestamp end,
        market::Timeframe timeframe
    ) noexcept
        : instrument_id_(instrument_id),
          start_(start),
          end_(end),
          timeframe_(timeframe) {}

    [[nodiscard]] constexpr market::InstrumentId instrument_id() const noexcept {
        return instrument_id_;
    }

    [[nodiscard]] constexpr market::Timestamp start() const noexcept {
        return start_;
    }

    [[nodiscard]] constexpr market::Timestamp end() const noexcept {
        return end_;
    }

    [[nodiscard]] constexpr market::Timeframe timeframe() const noexcept {
        return timeframe_;
    }

private:
    market::InstrumentId instrument_id_;
    market::Timestamp start_;
    market::Timestamp end_;
    market::Timeframe timeframe_;
};

} // namespace quantforge::marketdata
