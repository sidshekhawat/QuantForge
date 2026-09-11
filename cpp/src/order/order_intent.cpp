#include "quantforge/order/order_intent.hpp"

namespace quantforge::order {

OrderIntent::OrderIntent(
    market::InstrumentId instrument_id,
    OrderSide side,
    market::Quantity quantity) noexcept
    : instrument_id_(instrument_id),
      side_(side),
      quantity_(quantity) {}

market::InstrumentId OrderIntent::instrument_id() const noexcept {
    return instrument_id_;
}

OrderSide OrderIntent::side() const noexcept {
    return side_;
}

market::Quantity OrderIntent::quantity() const noexcept {
    return quantity_;
}

} // namespace quantforge::order
