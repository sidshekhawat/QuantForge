#pragma once

#include "quantforge/market/instrument_id.hpp"
#include "quantforge/market/timestamp.hpp"

#include <cstdint>

namespace quantforge::signal {

enum class SignalDirection {
    Buy,
    Sell,
    Hold
};

class Signal {
public:
    Signal(
        market::InstrumentId instrument_id,
        market::Timestamp timestamp,
        SignalDirection direction,
        std::uint8_t strength
    );

    [[nodiscard]] market::InstrumentId instrument_id() const noexcept;

    [[nodiscard]] market::Timestamp timestamp() const noexcept;

    [[nodiscard]] SignalDirection direction() const noexcept;

    [[nodiscard]] std::uint8_t strength() const noexcept;

private:
    market::InstrumentId instrument_id_;
    market::Timestamp timestamp_;
    SignalDirection direction_;
    std::uint8_t strength_;
};

} // namespace quantforge::signal
