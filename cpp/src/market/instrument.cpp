#include "quantforge/market/instrument.hpp"

namespace quantforge::market {

Instrument::Instrument(
    InstrumentId id,
    std::string symbol,
    AssetClass asset_class,
    std::string exchange,
    std::string currency,
    Price tick_size,
    Quantity lot_size
)
    : id_(id),
      symbol_(std::move(symbol)),
      asset_class_(asset_class),
      exchange_(std::move(exchange)),
      currency_(std::move(currency)),
      tick_size_(tick_size),
      lot_size_(lot_size) {}

InstrumentId Instrument::id() const noexcept {
    return id_;
}

const std::string& Instrument::symbol() const noexcept {
    return symbol_;
}

AssetClass Instrument::asset_class() const noexcept {
    return asset_class_;
}

const std::string& Instrument::exchange() const noexcept {
    return exchange_;
}

const std::string& Instrument::currency() const noexcept {
    return currency_;
}

Price Instrument::tick_size() const noexcept {
    return tick_size_;
}

Quantity Instrument::lot_size() const noexcept {
    return lot_size_;
}

} // namespace quantforge::market