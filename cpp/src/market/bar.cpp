#include "quantforge/market/bar.hpp"

namespace quantforge::market {

Bar::Bar(
    InstrumentId instrument_id,
    Timestamp timestamp,
    Timeframe timeframe,
    Price open,
    Price high,
    Price low,
    Price close,
    Quantity volume
)
    : instrument_id_(instrument_id),
      timestamp_(timestamp),
      timeframe_(timeframe),
      open_(open),
      high_(high),
      low_(low),
      close_(close),
      volume_(volume) {}

InstrumentId Bar::instrument_id() const noexcept {
    return instrument_id_;
}

Timestamp Bar::timestamp() const noexcept {
    return timestamp_;
}

Timeframe Bar::timeframe() const noexcept {
    return timeframe_;
}

Price Bar::open() const noexcept {
    return open_;
}

Price Bar::high() const noexcept {
    return high_;
}

Price Bar::low() const noexcept {
    return low_;
}

Price Bar::close() const noexcept {
    return close_;
}

Quantity Bar::volume() const noexcept {
    return volume_;
}

} // namespace quantforge::market