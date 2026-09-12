#include "quantforge/order/order.hpp"

#include <stdexcept>
#include <utility>

namespace quantforge::order {

Order::Order(
    OrderId id,
    market::InstrumentId instrument_id,
    OrderSide side,
    market::Quantity quantity,
    OrderType type,
    std::optional<market::Price> limit_price)
    : id_(id),
      instrument_id_(instrument_id),
      side_(side),
      quantity_(quantity),
      type_(type),
      limit_price_(std::move(limit_price))
{
    if (id_ == 0) {
        throw std::invalid_argument(
            "Order ID must be non-zero."
        );
    }

    if (type_ == OrderType::Limit && !limit_price_.has_value()) {
        throw std::invalid_argument(
            "Limit order requires a limit price."
        );
    }

    if (type_ == OrderType::Market && limit_price_.has_value()) {
        throw std::invalid_argument(
            "Market order cannot have a limit price."
        );
    }
}

OrderId Order::id() const noexcept {
    return id_;
}

market::InstrumentId Order::instrument_id() const noexcept {
    return instrument_id_;
}

OrderSide Order::side() const noexcept {
    return side_;
}

market::Quantity Order::quantity() const noexcept {
    return quantity_;
}

OrderType Order::type() const noexcept {
    return type_;
}

const std::optional<market::Price>&
Order::limit_price() const noexcept {
    return limit_price_;
}

} // namespace quantforge::order
