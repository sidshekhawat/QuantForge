#pragma once

#include "quantforge/market/instrument_id.hpp"
#include "quantforge/market/price.hpp"
#include "quantforge/market/quantity.hpp"
#include "quantforge/market/timeframe.hpp"
#include "quantforge/market/timestamp.hpp"

namespace quantforge::market {

class Bar {
public:
    Bar(
        InstrumentId instrument_id,
        Timestamp timestamp,
        Timeframe timeframe,
        Price open,
        Price high,
        Price low,
        Price close,
        Quantity volume
    );

    [[nodiscard]] InstrumentId instrument_id() const noexcept;

    [[nodiscard]] Timestamp timestamp() const noexcept;

    [[nodiscard]] Timeframe timeframe() const noexcept;

    [[nodiscard]] Price open() const noexcept;

    [[nodiscard]] Price high() const noexcept;

    [[nodiscard]] Price low() const noexcept;

    [[nodiscard]] Price close() const noexcept;

    [[nodiscard]] Quantity volume() const noexcept;

private:
    InstrumentId instrument_id_;
    Timestamp timestamp_;
    Timeframe timeframe_;

    Price open_;
    Price high_;
    Price low_;
    Price close_;

    Quantity volume_;
};

} // namespace quantforge::market