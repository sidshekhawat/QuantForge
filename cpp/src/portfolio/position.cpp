#include "quantforge/portfolio/position.hpp"

namespace quantforge::portfolio {

Position::Position(
    market::InstrumentId instrument_id,
    PositionSide side,
    market::Quantity quantity,
    market::Price average_entry_price) noexcept
    : instrument_id_(instrument_id),
      side_(side),
      quantity_(quantity),
      average_entry_price_(average_entry_price) {}

market::InstrumentId Position::instrument_id() const noexcept {
    return instrument_id_;
}

PositionSide Position::side() const noexcept {
    return side_;
}

market::Quantity Position::quantity() const noexcept {
    return quantity_;
}

market::Price Position::average_entry_price() const noexcept {
    return average_entry_price_;
}

} // namespace quantforge::portfolio
