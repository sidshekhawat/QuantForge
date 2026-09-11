#pragma once

#include "quantforge/market/instrument_id.hpp"
#include "quantforge/market/quantity.hpp"

namespace quantforge::order {

enum class OrderSide {
    Buy,
    Sell
};

class OrderIntent {
public:
    OrderIntent(
        market::InstrumentId instrument_id,
        OrderSide side,
        market::Quantity quantity
    ) noexcept;

    [[nodiscard]] market::InstrumentId instrument_id() const noexcept;

    [[nodiscard]] OrderSide side() const noexcept;

    [[nodiscard]] market::Quantity quantity() const noexcept;

private:
    market::InstrumentId instrument_id_;
    OrderSide side_;
    market::Quantity quantity_;
};

} // namespace quantforge::order
