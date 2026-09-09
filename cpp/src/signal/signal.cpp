#include "quantforge/signal/signal.hpp"

namespace quantforge::signal {

Signal::Signal(
    market::InstrumentId instrument_id,
    market::Timestamp timestamp,
    SignalDirection direction,
    std::uint8_t strength)
    : instrument_id_(instrument_id),
      timestamp_(timestamp),
      direction_(direction),
      strength_(strength) {}

market::InstrumentId Signal::instrument_id() const noexcept {
    return instrument_id_;
}

market::Timestamp Signal::timestamp() const noexcept {
    return timestamp_;
}

SignalDirection Signal::direction() const noexcept {
    return direction_;
}

std::uint8_t Signal::strength() const noexcept {
    return strength_;
}

} // namespace quantforge::signal
