#pragma once

#include "quantforge/market/instrument_id.hpp"
#include "quantforge/market/price.hpp"
#include "quantforge/market/quantity.hpp"

namespace quantforge::portfolio {

enum class PositionSide {
    Flat,
    Long,
    Short
};

class Position {
public:
    Position(
        market::InstrumentId instrument_id,
        PositionSide side,
        market::Quantity quantity,
        market::Price average_entry_price
    ) noexcept;

    [[nodiscard]] market::InstrumentId instrument_id() const noexcept;

    [[nodiscard]] PositionSide side() const noexcept;

    [[nodiscard]] market::Quantity quantity() const noexcept;

    [[nodiscard]] market::Price average_entry_price() const noexcept;

private:
    market::InstrumentId instrument_id_;
    PositionSide side_;
    market::Quantity quantity_;
    market::Price average_entry_price_;
};

} // namespace quantforge::portfolio
