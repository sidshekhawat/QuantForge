#include "quantforge/execution/fill.hpp"

#include <stdexcept>

namespace quantforge::execution {

Fill::Fill(
    FillId fill_id,
    order::OrderId order_id,
    market::InstrumentId instrument_id,
    order::OrderSide side,
    market::Quantity quantity,
    market::Price price,
    market::Timestamp timestamp)
    : id_(fill_id),
      order_id_(order_id),
      instrument_id_(instrument_id),
      side_(side),
      quantity_(quantity),
      price_(price),
      timestamp_(timestamp)
{
    if (id_ == 0) {
        throw std::invalid_argument(
            "Fill ID must be non-zero."
        );
    }

    if (order_id_ == 0) {
        throw std::invalid_argument(
            "Fill order ID must be non-zero."
        );
    }
}

FillId Fill::id() const noexcept {
    return id_;
}

order::OrderId Fill::order_id() const noexcept {
    return order_id_;
}

market::InstrumentId Fill::instrument_id() const noexcept {
    return instrument_id_;
}

order::OrderSide Fill::side() const noexcept {
    return side_;
}

market::Quantity Fill::quantity() const noexcept {
    return quantity_;
}

market::Price Fill::price() const noexcept {
    return price_;
}

market::Timestamp Fill::timestamp() const noexcept {
    return timestamp_;
}

} // namespace quantforge::execution
