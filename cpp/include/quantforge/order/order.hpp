#pragma once

#include "quantforge/market/instrument_id.hpp"
#include "quantforge/market/price.hpp"
#include "quantforge/market/quantity.hpp"
#include "quantforge/order/order_intent.hpp"

#include <cstdint>
#include <optional>

namespace quantforge::order {

using OrderId = std::uint64_t;

enum class OrderType {
    Market,
    Limit
};

class Order {
public:
    Order(
        OrderId id,
        market::InstrumentId instrument_id,
        OrderSide side,
        market::Quantity quantity,
        OrderType type,
        std::optional<market::Price> limit_price = std::nullopt
    );

    [[nodiscard]] OrderId id() const noexcept;

    [[nodiscard]] market::InstrumentId instrument_id() const noexcept;

    [[nodiscard]] OrderSide side() const noexcept;

    [[nodiscard]] market::Quantity quantity() const noexcept;

    [[nodiscard]] OrderType type() const noexcept;

    [[nodiscard]] const std::optional<market::Price>&
    limit_price() const noexcept;

private:
    OrderId id_;
    market::InstrumentId instrument_id_;
    OrderSide side_;
    market::Quantity quantity_;
    OrderType type_;
    std::optional<market::Price> limit_price_;
};

} // namespace quantforge::order
