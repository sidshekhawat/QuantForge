#pragma once

#include "quantforge/market/instrument_id.hpp"
#include "quantforge/market/price.hpp"
#include "quantforge/market/quantity.hpp"
#include "quantforge/market/timestamp.hpp"
#include "quantforge/order/order.hpp"

#include <cstdint>

namespace quantforge::execution {

using FillId = std::uint64_t;

class Fill {
public:
    Fill(
        FillId fill_id,
        order::OrderId order_id,
        market::InstrumentId instrument_id,
        order::OrderSide side,
        market::Quantity quantity,
        market::Price price,
        market::Timestamp timestamp
    );

    [[nodiscard]] FillId id() const noexcept;
    [[nodiscard]] order::OrderId order_id() const noexcept;
    [[nodiscard]] market::InstrumentId instrument_id() const noexcept;
    [[nodiscard]] order::OrderSide side() const noexcept;
    [[nodiscard]] market::Quantity quantity() const noexcept;
    [[nodiscard]] market::Price price() const noexcept;
    [[nodiscard]] market::Timestamp timestamp() const noexcept;

private:
    FillId id_;
    order::OrderId order_id_;
    market::InstrumentId instrument_id_;
    order::OrderSide side_;
    market::Quantity quantity_;
    market::Price price_;
    market::Timestamp timestamp_;
};

} // namespace quantforge::execution
